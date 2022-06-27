#include "Executor.h"
#include "SQLParser.h"
#include <vector>
#include "Type.h"
#include "fudgeDBError.h"
#include "TupleDesc.h"
#include "Table.h"
#include "TableCatalog.h"
#include "FudgeDB.h"
#include "Tuple.h"
#include "iostream"
#include "Field.h"
#include "TupleIterator.h"

using namespace fudgeDB;

std::string Executor::executeStatement(const hsql::SQLStatement* statement){
    switch(statement->type()){
        case hsql::StatementType::kStmtCreate:
            return executeCreateStatement((hsql::CreateStatement*)(statement));
        case hsql::StatementType::kStmtInsert:
            return executeInsertStatement((hsql::InsertStatement*)(statement));
        case hsql::StatementType::kStmtSelect:
            return executeSelectStatement((hsql::SelectStatement*)(statement));
        default:
            throw fudgeError("unsupported query");
    }
}
std::string Executor::executeCreateStatement(const hsql::CreateStatement* statement){
    switch(statement->type){
        case hsql::CreateType::kCreateTable:
            return createTableHelper(statement);
        default:
            throw fudgeError("unsupported query");
    }
}
std::string Executor::executeInsertStatement(const hsql::InsertStatement* statement){
    switch(statement->type){
        case hsql::InsertType::kInsertValues:
            return insertValueHelper(statement);
        default:
            throw fudgeError("unsupported query");
    }
}
std::string Executor::executeSelectStatement(const hsql::SelectStatement* statement){
    std::string tableName = statement->fromTable->name;
    std::cout<<"From table name: "<<tableName<<std::endl;
    auto table = FudgeDB::getFudgDB()->getTableCatalog()->getTable(tableName);
    if(table == nullptr){
        return tableName + " not found in the database";
    }
    return this->tupleIteratorToString(table->getIterator());
}

std::string Executor::createTableHelper(const hsql::CreateStatement* statement){
    std::string tableName(statement->tableName);
    std::vector<std::string> names;
    std::vector<Type*> types;
    for(auto col : *(statement->columns)){
        names.push_back(col->name);
        switch(col->type.data_type){
            case hsql::DataType::INT:
                types.push_back(new IntType());
                break;
            case hsql::DataType::CHAR:
                types.push_back(new StringType(col->type.length));
                break;
            default:
                throw fudgeError("unsupported data type");
        }
    }
    for(auto constrain : *(statement->tableConstraints)){
        //TODO: add support to primary key for indexing
    }
    TupleDesc* tupleDesc = new TupleDesc(names, types);
    if(FudgeDB::getFudgDB()->getTableCatalog()->getTable(tableName) != nullptr){
        throw fudgeError("Table already exists");
    }
    FudgeDB::getFudgDB()->getTableCatalog()->insertTable(tableName, new Table(tableName, tupleDesc, 0, Tuple::NONEXTAVAIL));
    return "Created table successfully";
}

std::string Executor::insertValueHelper(const hsql::InsertStatement* statement){
    // currently only support insert the values in the order of the table tuple Desc
    std::string tableName = statement->tableName;
    auto tableCatalog = FudgeDB::getFudgDB()->getTableCatalog();
    auto table = tableCatalog->getTable(tableName);
    if(table == nullptr){
        return "Table not exist";
    }
    auto tupleDesc = table->getTupleDesc();
    if(tupleDesc->getLength() != statement->values->size()){
        return "Cannot fit table columns, column number error";
    }
    for(int i = 0; i < tupleDesc->getLength(); i++){
        auto val = statement->values->at(i);
        switch(val->type){
            case hsql::ExprType::kExprLiteralInt:
                if(tupleDesc->getType(i)->getType() != TypeEnum::IntTypeEnum) return "Cannot fit table columns, type error";
                break;
            case hsql::ExprType::kExprLiteralString:
                if(tupleDesc->getType(i)->getType() != TypeEnum::StringTypeEnum) return "Cannot fit table columns, type error";
                break;
            default:
                return "Unsupported data type";
        }
    }
    std::vector<Field*> fields;
    for(int i = 0; i < tupleDesc->getLength(); i++){
        auto val = statement->values->at(i);
        switch(val->type){
            case hsql::ExprType::kExprLiteralInt:
                fields.push_back(new IntField(val->ival));
                break;
            case hsql::ExprType::kExprLiteralString:
                fields.push_back(new StringField(val->name));
                break;
        }
    }
    auto tuple = new Tuple(tupleDesc, fields);
    table->insertTuple(tuple);
    return "Insert successfully";
}

std::string Executor::tupleIteratorToString(TupleIterator* tupleIterator){
    std::string res = tupleIterator->getTupleDesc()->toString() + '\n';
    tupleIterator->open();
    while(tupleIterator->hasNext()){
        res += tupleIterator->next()->toString() + '\n';
    }
    tupleIterator->close();
    return res;
}
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
    return "";
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
    
}
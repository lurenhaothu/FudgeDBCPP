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

#include "TupleIterator.h"
#include "execution/WhereIterator.h"
#include "execution/JoinIterator.h"
#include "execution/AggregationIterator.h"
#include "execution/ExecutionUtility.h"
#include "execution/OrderIterator.h"
#include "execution/LimitIterator.h"
#include "execution/SelectIterator.h"

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
    auto tupleIterator = this->executeSelectToIterator(statement);
    return this->tupleIteratorToString(tupleIterator);
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
    std::vector<Field*> fields(tupleDesc->getLength(), nullptr);
    for(int i = 0; i < statement->values->size(); i++){
        auto field = ExecutionUtility::getField(nullptr, statement->values->at(i), nullptr);
        int index = i;
        if(statement->columns != nullptr && i < statement->columns->size()){
            std::string colName = statement->columns->at(i);
            index = tupleDesc->getIndex("", colName);
            if(index == -1){
                throw fudgeError("insert error, cannot find the column");
            }
        }
        if(tupleDesc->getType(index)->getType() != field->getType()){
            throw fudgeError("column type error");
        }
        fields[index] = field;
    }
    for(auto field : fields){
        if(field == nullptr){
            throw fudgeError("null column not supported");
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
        auto tuple = tupleIterator->next();
        res += tuple->toString() + '\n';
        delete tuple;
    }
    tupleIterator->close();
    return res;
}

TupleIterator* Executor::executeSelectToIterator(const hsql::SelectStatement* statement){
    //execution order
    //FROM & JOINs determine & filter rows
    //WHERE more filters on the rows
    //GROUP BY combines those rows into groups
    //HAVING filters groups
    //ORDER BY arranges the remaining rows/groups
    //LIMIT filters on the remaining rows/groups
    auto colAliasMap = ExecutionUtility::getColAliasMap(statement->selectList);

    TupleIterator* fromIterator = executeFromTableRef(statement->fromTable);
    auto whereClause = statement->whereClause;
    TupleIterator* whereIterator = fromIterator;
    if(whereClause != nullptr) whereIterator = new WhereIterator(whereClause, fromIterator);

    std::vector<hsql::Expr*> aggregates;
    std::vector<hsql::Expr*> groupby;
    hsql::Expr* having = nullptr;
    ExecutionUtility::getAggregationExpr(statement->selectList, aggregates);
    if(statement->groupBy != nullptr){
        ExecutionUtility::getAggregationExpr(statement->groupBy->columns, aggregates);
    }
    if(statement->order != nullptr){
        for(auto orderDesc : *statement->order){
            ExecutionUtility::getAggregationExpr(orderDesc->expr, aggregates);
        }
    }
    if(statement->groupBy != nullptr){
        for(auto expr : *statement->groupBy->columns){
            groupby.push_back(expr);
        }
        having = statement->groupBy->having;
    }
    if(having != nullptr){
        ExecutionUtility::getAggregationExpr(having, aggregates);
    }
    auto aggrIterator = whereIterator;
    if(aggregates.size() != 0){
        aggrIterator = new AggregationIterator(aggregates, groupby, having, whereIterator, colAliasMap);
    }
    auto orderIterator = aggrIterator;
    if(statement->order != nullptr){
        orderIterator = new OrderIterator(statement->order, aggrIterator, colAliasMap);
    }
    auto limitIterator = orderIterator;
    if(statement->limit != nullptr){
        limitIterator = new LimitIterator(statement->limit, orderIterator);
    }
    auto selectIterator = new SelectIterator(statement->selectList, limitIterator, colAliasMap);
    return selectIterator;
}

TupleIterator* Executor::executeFromTableRef(const hsql::TableRef* tableRef){
    Table* table;
    TupleIterator* fromIterator = nullptr;
    TupleIterator* left;
    TupleIterator* right;
    std::vector<TupleIterator*> tupleIterators;
    switch(tableRef->type){
        case hsql::TableRefType::kTableName:
            if(tableRef->alias == nullptr){
                table = FudgeDB::getFudgDB()->getTableCatalog()->
                    getTable(tableRef->name);
                if(table == nullptr) throw fudgeError("table not found");
                fromIterator = FudgeDB::getFudgDB()->getTableCatalog()->
                    getTable(tableRef->name)->getIterator();
            }else{
                table = FudgeDB::getFudgDB()->getTableCatalog()->
                    getTable(tableRef->name);
                if(table == nullptr) throw fudgeError("table not found");
                fromIterator = FudgeDB::getFudgDB()->getTableCatalog()->
                    getTable(tableRef->name)->getIterator(tableRef->alias->name);
            }
            break;
        case hsql::TableRefType::kTableCrossProduct:
            for(auto table : *(tableRef->list)){
                tupleIterators.push_back(executeFromTableRef(table));
            }
            fromIterator = new JoinIterator(&tupleIterators);
            break;
        case hsql::TableRefType::kTableJoin:
            left = executeFromTableRef(tableRef->join->left);
            right = executeFromTableRef(tableRef->join->right);
            fromIterator = new JoinIterator(left, right, tableRef->join->condition);
            break;
        case hsql::TableRefType::kTableSelect:
            //TODO
            break;
        default:
            throw fudgeError("Unsupported select from table");
    }
    return fromIterator;
}
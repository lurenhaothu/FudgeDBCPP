#include "ExecutionUtility.h"
#include "SQLParser.h"
#include "general/fudgeDBError.h"
#include "general/Tuple.h"
#include "general/TupleDesc.h"
#include "iostream"
#include "general/Type.h"

using namespace fudgeDB;

bool ExecutionUtility::filter(Tuple* tuple, hsql::Expr* expr, std::unordered_map<std::string, hsql::Expr*>* aliasMap){
    if(expr == nullptr) return true;
    if(expr->type != hsql::ExprType::kExprOperator){
        throw fudgeError("unsupported where clause type");
    }
    switch(expr->opType){
        case hsql::OperatorType::kOpAnd:
            return filter(tuple, expr->expr, aliasMap) && filter(tuple, expr->expr2, aliasMap);
        case hsql::OperatorType::kOpOr:
            return filter(tuple, expr->expr, aliasMap) || filter(tuple, expr->expr2, aliasMap);
        case hsql::OperatorType::kOpNot:
            return !filter(tuple, expr->expr, aliasMap);
        case hsql::OperatorType::kOpEquals:
            return binaryFilter(tuple, expr, Op::equals, aliasMap);
        case hsql::OperatorType::kOpNotEquals:
            return !binaryFilter(tuple, expr, Op::equals, aliasMap);
        case hsql::OperatorType::kOpGreater:
            return binaryFilter(tuple, expr, Op::greater, aliasMap);
        case hsql::OperatorType::kOpGreaterEq:
            return !binaryFilter(tuple, expr, Op::less, aliasMap);
        case hsql::OperatorType::kOpLess:
            return binaryFilter(tuple, expr, Op::less, aliasMap);
        case hsql::OperatorType::kOpLessEq:
            return !binaryFilter(tuple, expr, Op::greater, aliasMap);
        default:
            throw fudgeError("unsupported where clause type");
    }
}

bool ExecutionUtility::binaryFilter(Tuple* tuple, hsql::Expr* expr, Op op, std::unordered_map<std::string, hsql::Expr*>* aliasMap){
    Field* field1 = getField(tuple, expr->expr, aliasMap);
    Field* field2 = getField(tuple, expr->expr2, aliasMap);
    bool res = Field::filter(field1, field2, op);
    delete field1;
    delete field2;
    return res;
}

Field* ExecutionUtility::getField(Tuple* tuple, hsql::Expr* expr, std::unordered_map<std::string, hsql::Expr*>* aliasMap){
    switch(expr->type){
        case hsql::ExprType::kExprOperator:
            return getFieldByCalculation(tuple, expr, aliasMap);
        case hsql::ExprType::kExprLiteralInt:
            return new IntField(expr->ival);
        case hsql::ExprType::kExprLiteralString:
            return new StringField(expr->name);
        case hsql::ExprType::kExprColumnRef:
            return getFieldByColumnRef(tuple, expr, aliasMap);
        case hsql::ExprType::kExprFunctionRef:
            return getFieldByAggregate(tuple, expr, aliasMap);
        default:
            throw fudgeError("unsupported where clause type");
    }
}

Field* ExecutionUtility::getFieldByColumnRef(Tuple* tuple, hsql::Expr* expr, std::unordered_map<std::string, hsql::Expr*>* aliasMap){
    std::string alias = expr->table == nullptr ? "" : expr->table;
    std::string colName = expr->name == nullptr ? "" : expr->name;
    //std::cout<<alias<<", "<<colName<<std::endl;
    if(aliasMap != nullptr && aliasMap->find(colName) != aliasMap->end()){
        return ExecutionUtility::getField(tuple, aliasMap->at(colName), aliasMap);
    }
    int index = tuple->getTupleDesc()->getIndex(alias, colName);
    if(index == -1){
        throw fudgeError("column name not matched");
    }
    return Field::copy(tuple->getField(index));
}

Field* ExecutionUtility::getFieldByCalculation(Tuple* tuple, hsql::Expr* expr, std::unordered_map<std::string, hsql::Expr*>* aliasMap){
    Field* field1 = getField(tuple, expr->expr, aliasMap);
    Field* field2 = getField(tuple, expr->expr2, aliasMap);
    Field* res;
    switch(expr->opType){
        case hsql::OperatorType::kOpPlus:
            res = Field::calculate(field1, field2, Op::plus);
            break;
        case hsql::OperatorType::kOpMinus:
            res = Field::calculate(field1, field2, Op::minus);
            break;
        case hsql::OperatorType::kOpAsterisk:
            res = Field::calculate(field1, field2, Op::times);
            break;
        case hsql::OperatorType::kOpSlash:
            res = Field::calculate(field1, field2, Op::divide);
            break;
        default:
            throw fudgeError("unsupported calculation operator");
    }
    delete field1;
    delete field2;
    return res;
}

std::vector<std::string> ExecutionUtility::getColName(hsql::Expr* expr, TupleDesc* tupleDesc, 
    std::unordered_map<std::string, hsql::Expr*>* aliasMap){
    std::vector<std::string> res(3, "");
    if(expr == nullptr) return res;
    else if(expr->type == hsql::ExprType::kExprColumnRef){
        if(aliasMap->find(expr->name) != aliasMap->end()){
            return ExecutionUtility::getColName(aliasMap->at(expr->name), tupleDesc, aliasMap);
        }else{
            std::string tableName = expr->table == nullptr ? "" : expr->table;
            std::string colName = expr->name == nullptr ? "" : expr->name;
            int index = tupleDesc->getIndex(tableName, colName);
            if(index == -1) throw fudgeError("column not found");
            res[0] = tupleDesc->getTableName(index);
            res[1] = tupleDesc->getAlias(index);
            res[2] = tupleDesc->getName(index);
            return res;
        }
    }else if(expr->type == hsql::ExprType::kExprFunctionRef){
        if(expr->exprList == nullptr || expr->exprList->size() != 1 || expr->name == nullptr)
            throw fudgeError("aggr func not supported");
        auto childRes = ExecutionUtility::getColName(expr->exprList->at(0), tupleDesc, aliasMap);
        res[0] = childRes[0];
        res[1] = childRes[1];
        std::string funcName = std::string(expr->name);
        //std::cout<<funcName<<" "<<funcName.size()<<std::endl;
        res[2] = funcName + "(" + childRes[2] + ")";
        return res;
    }else if(expr->type == hsql::ExprType::kExprOperator){
        std::string opStr;
        switch(expr->opType){
            case hsql::OperatorType::kOpAsterisk:
                opStr = "*";
                break;
            case hsql::OperatorType::kOpSlash:
                opStr = "/";
                break;
            case hsql::OperatorType::kOpMinus:
                opStr = "-";
                break;
            case hsql::OperatorType::kOpPlus:
                opStr = "+";
                break;
            default:
                throw fudgeError("op not supported");
        }
        auto child1 = ExecutionUtility::getColName(expr->expr, tupleDesc, aliasMap);
        auto child2 = ExecutionUtility::getColName(expr->expr2, tupleDesc, aliasMap);
        if(child1[0].size() == 0) res[0] = child2[0];
        if(child2[0].size() == 0) res[0] = child1[0];
        if(child1[0].size() != 0 && child2[0].size() != 0) res[0] = "";
        if(child1[1].size() == 0) res[1] = child2[1];
        if(child2[1].size() == 0) res[1] = child1[1];
        if(child1[1].size() != 0 && child2[1].size() != 0) res[1] = "";
        res[2] = child1[2] + opStr + child2[2];
        return res;
    }else if(expr->type == hsql::ExprType::kExprLiteralInt){
        res[2] = std::to_string(expr->ival);
        return res;
    }else if(expr->type == hsql::ExprType::kExprLiteralString){
        res[2] = expr->name;
        return res;
    }else{
        throw fudgeError("expr type not supported");
    }
}
void ExecutionUtility::getAggregationExpr(std::vector<hsql::Expr*>* list, 
    std::vector<hsql::Expr*>& aggrList){
    for(auto expr : *list){
        ExecutionUtility::getAggregationExpr(expr, aggrList);
    }
    return;
}
void ExecutionUtility::getAggregationExpr(hsql::Expr* expr, 
    std::vector<hsql::Expr*>& aggrList){
    if(expr == nullptr) return;
    else if(expr->type == hsql::ExprType::kExprFunctionRef){
        aggrList.push_back(expr);
    }else{
        ExecutionUtility::getAggregationExpr(expr->expr, aggrList);
        ExecutionUtility::getAggregationExpr(expr->expr2, aggrList);
    }
}
std::unordered_map<std::string, hsql::Expr*>* ExecutionUtility::getColAliasMap(std::vector<hsql::Expr*>* selectList){
    auto aliasMap = new std::unordered_map<std::string, hsql::Expr*>();
    for(auto expr : *selectList){
        if(expr->alias != nullptr){
            std::string key = expr->alias;
            aliasMap->at(key) = expr;
        }
    }
    return aliasMap;
}

Field* ExecutionUtility::getFieldByAggregate(Tuple* tuple, hsql::Expr* expr, std::unordered_map<std::string, hsql::Expr*>* aliasMap){
    auto colNames = ExecutionUtility::getColName(expr, tuple->getTupleDesc(), aliasMap);
    auto index = tuple->getTupleDesc()->getIndex(colNames[0], colNames[2]);
    if(index == -1) index = tuple->getTupleDesc()->getIndex(colNames[1], colNames[2]);
    if(index == -1) throw fudgeError("Cannot find column");
    return Field::copy(tuple->getField(index));
}

Type* ExecutionUtility::getColType(hsql::Expr* expr, TupleDesc* tupleDesc, 
    std::unordered_map<std::string, hsql::Expr*>* aliasMap){
    if(expr == nullptr) return nullptr;
    else if(expr->type == hsql::ExprType::kExprColumnRef){
        if(aliasMap->find(expr->name) != aliasMap->end()){
            return ExecutionUtility::getColType(aliasMap->at(expr->name), tupleDesc, aliasMap);
        }else{
            std::string tableName = expr->table == nullptr ? "" : expr->table;
            std::string colName = expr->name == nullptr ? "" : expr->name;
            int index = tupleDesc->getIndex(tableName, colName);
            if(index == -1) throw fudgeError("column not found");
            return Type::copy(tupleDesc->getType(index));
        }
    }else if(expr->type == hsql::ExprType::kExprFunctionRef){
        return new IntType();
    }else if(expr->type == hsql::ExprType::kExprOperator){
        Type* type;
        auto child1 = ExecutionUtility::getColType(expr->expr, tupleDesc, aliasMap);
        auto child2 = ExecutionUtility::getColType(expr->expr2, tupleDesc, aliasMap);
        if(child1 == nullptr) type = child2;
        else type = child1;
        return type;
    }else if(expr->type == hsql::ExprType::kExprLiteralInt){
        return new IntType();
    }else if(expr->type == hsql::ExprType::kExprLiteralString){
        return new StringType(0);
    }else{
        throw fudgeError("expr type not supported");
    }
}
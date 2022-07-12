#include "WhereIterator.h"
#include "general/TupleDesc.h"
#include "general/Tuple.h"
#include "iostream"

using namespace fudgeDB;

WhereIterator::WhereIterator(hsql::Expr *whereClause, TupleIterator* tupleIterator){
    this->whereClause = whereClause;
    this->tupleIterator = tupleIterator;
}

Tuple* WhereIterator::fetchNext(){
    while(tupleIterator->hasNext()){
        auto tuple = tupleIterator->next();
        if(this->filter(tuple, this->whereClause)){
            return tuple;
        }
    }
    return nullptr;
}
void WhereIterator::open(){
    tupleIterator->open();
}
void WhereIterator::close(){
    tupleIterator->close();
}
void WhereIterator::rewind(){
    tupleIterator->rewind();
}
TupleDesc* WhereIterator::getTupleDesc(){
    return tupleIterator->getTupleDesc();
}
bool WhereIterator::filter(Tuple* tuple, hsql::Expr* expr){
    if(expr->type != hsql::ExprType::kExprOperator){
        throw fudgeError("unsupported where clause type");
    }
    switch(expr->opType){
        case hsql::OperatorType::kOpAnd:
            return filter(tuple, expr->expr) && filter(tuple, expr->expr2);
        case hsql::OperatorType::kOpOr:
            return filter(tuple, expr->expr) || filter(tuple, expr->expr2);
        case hsql::OperatorType::kOpNot:
            return !filter(tuple, expr->expr);
        case hsql::OperatorType::kOpEquals:
            return binaryFilter(tuple, expr, Op::equals);
        case hsql::OperatorType::kOpNotEquals:
            return !binaryFilter(tuple, expr, Op::equals);
        case hsql::OperatorType::kOpGreater:
            return binaryFilter(tuple, expr, Op::greater);
        case hsql::OperatorType::kOpGreaterEq:
            return !binaryFilter(tuple, expr, Op::less);
        case hsql::OperatorType::kOpLess:
            return binaryFilter(tuple, expr, Op::less);
        case hsql::OperatorType::kOpLessEq:
            return !binaryFilter(tuple, expr, Op::greater);
        default:
            throw fudgeError("unsupported where clause type");
    }
}

bool WhereIterator::binaryFilter(Tuple* tuple, hsql::Expr* expr, Op op){
    Field* field1 = getField(tuple, expr->expr);
    Field* field2 = getField(tuple, expr->expr2);
    bool res = Field::filter(field1, field2, op);
    delete field1;
    delete field2;
    return res;
}

Field* WhereIterator::getField(Tuple* tuple, hsql::Expr* expr){
    switch(expr->type){
        case hsql::ExprType::kExprOperator:
            return getFieldByCalculation(tuple, expr);
        case hsql::ExprType::kExprLiteralInt:
            return new IntField(expr->ival);
        case hsql::ExprType::kExprLiteralString:
            return new StringField(expr->name);
        case hsql::ExprType::kExprColumnRef:
            return getFieldByColumnRef(tuple, expr);
        default:
            throw fudgeError("unsupported where clause type");
    }
}

Field* WhereIterator::getFieldByColumnRef(Tuple* tuple, hsql::Expr* expr){
    std::string alias = expr->table == nullptr ? "" : expr->table;
    std::string colName = expr->name == nullptr ? "" : expr->name;
    //std::cout<<alias<<", "<<colName<<std::endl;
    int index = this->getTupleDesc()->getIndex(alias, colName);
    if(index == -1){
        throw fudgeError("column name not matched");
    }
    return Field::copy(tuple->getField(index));
}

Field* WhereIterator::getFieldByCalculation(Tuple* tuple, hsql::Expr* expr){
    Field* field1 = getField(tuple, expr->expr);
    Field* field2 = getField(tuple, expr->expr2);
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
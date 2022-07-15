#include "SelectIterator.h"
#include "general/TupleDesc.h"
#include "ExecutionUtility.h"
#include "general/Tuple.h"
#include "iostream"

using namespace fudgeDB;


SelectIterator::SelectIterator(std::vector<hsql::Expr*>* selectList, TupleIterator* tupleIterator, 
    std::unordered_map<std::string, hsql::Expr*>* aliasMap){
    this->selectList = new std::vector<hsql::Expr*>();
    for(auto expr : *selectList){
        if(expr->type != hsql::ExprType::kExprStar) this->selectList->push_back(expr);
        else ExecutionUtility::replaceStar(expr, tupleIterator->getTupleDesc(), this->selectList);
    }
    this->tupleIterator = tupleIterator;
    this->aliasMap = aliasMap;
    std::vector<std::string> tableNames;
    std::vector<std::string> tableAlias;
    std::vector<std::string> colNames;
    std::vector<Type*> types;
    this->childTupleDesc = tupleIterator->getTupleDesc();
    for(auto expr : *this->selectList){
        auto names = ExecutionUtility::getColName(expr, childTupleDesc, aliasMap);
        tableNames.push_back(names[0]);
        tableAlias.push_back(names[1]);
        colNames.push_back(names[2]);
        auto type = ExecutionUtility::getColType(expr, childTupleDesc, aliasMap);
        types.push_back(type);
    }
    this->tupleDesc = new TupleDesc(colNames, types, tableNames, tableAlias);
    //std::cout<<tupleDesc->toString()<<std::endl;
}
SelectIterator::~SelectIterator(){
    delete tupleIterator;
    delete tupleDesc;
    delete selectList;
}
Tuple* SelectIterator::fetchNext(){
    if(tupleIterator->hasNext()){
        auto tuple = tupleIterator->next();
        auto selectTuple = this->getTuple(tuple);
        delete tuple;
        return selectTuple;
    }else{
        return nullptr;
    }
}
void SelectIterator::open(){
    tupleIterator->open();
}
void SelectIterator::close(){
    tupleIterator->close();
}
void SelectIterator::rewind(){
    tupleIterator->rewind();
}
TupleDesc* SelectIterator::getTupleDesc(){
    return this->tupleDesc;
}

Tuple* SelectIterator::getTuple(Tuple* tuple){
    std::vector<Field*> fields;
    for(auto expr : *selectList){
        auto field = ExecutionUtility::getField(tuple, expr, aliasMap);
        fields.push_back(field);
    }
    return new Tuple(tupleDesc, fields);
}
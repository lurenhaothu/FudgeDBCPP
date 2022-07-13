#include "JoinIterator.h"
#include "general/TupleDesc.h"
#include "general/Tuple.h"
#include "ExecutionUtility.h"
#include "iostream"

using namespace fudgeDB;

JoinIterator::JoinIterator(TupleIterator* leftItr, TupleIterator* rightItr, hsql::Expr* condition){
    this->leftItr = leftItr;
    this->rightItr = rightItr;
    this->condition = condition;
    this->tupleDesc = new TupleDesc(leftItr->getTupleDesc(), rightItr->getTupleDesc());
    leftTuple = nullptr;
}
JoinIterator::JoinIterator(std::vector<TupleIterator*>* tupleIterators){
    if(tupleIterators->size() == 2){
        this->leftItr = tupleIterators->at(0);
        this->rightItr = tupleIterators->at(1);
        this->condition = nullptr;
    }else{
        this->rightItr = tupleIterators->back();
        tupleIterators->pop_back();
        this->leftItr = new JoinIterator(tupleIterators);
        this->condition = nullptr;
    }
    this->tupleDesc = new TupleDesc(leftItr->getTupleDesc(), rightItr->getTupleDesc());
    leftTuple = nullptr;
}
JoinIterator::~JoinIterator(){
    delete leftTuple;
    delete leftItr;
    delete rightItr;
    delete condition;
    delete tupleDesc;
}
Tuple* JoinIterator::fetchNext(){
    Tuple* tuple = fetchWithoutFilter();
    while(tuple != nullptr && !ExecutionUtility::filter(tuple, this->condition)){
        delete tuple;
        tuple = fetchWithoutFilter();
    }
    return tuple;
    
}
void JoinIterator::open(){
    leftItr->open();
    rightItr->open();
    if(leftItr->hasNext()){
        leftTuple = leftItr->next();
    }else{
        leftTuple = nullptr;
    }
}
void JoinIterator::close(){
    leftItr->close();
    rightItr->close();
    delete leftTuple;
}
void JoinIterator::rewind(){
    leftItr->rewind();
    rightItr->rewind();
    delete leftTuple;
    if(leftItr->hasNext()){
        leftTuple = leftItr->next();
    }else{
        leftTuple = nullptr;
    }
}
TupleDesc* JoinIterator::getTupleDesc(){
    return this->tupleDesc;
}

Tuple* JoinIterator::fetchWithoutFilter(){
    if(leftTuple == nullptr) return nullptr;
    while(!rightItr->hasNext()){
        if(leftItr->hasNext()){
            delete leftTuple;
            leftTuple = leftItr->next();
            rightItr->rewind();
        }else{
            delete leftTuple;
            leftTuple = nullptr;
            return nullptr;
        }
    }
    auto rightTuple = rightItr->next();
    auto res = new Tuple(leftTuple, rightTuple, this->tupleDesc);
    delete rightTuple;
    return res;
}
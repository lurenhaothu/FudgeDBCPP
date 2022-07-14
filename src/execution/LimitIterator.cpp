#include "LimitIterator.h"

using namespace fudgeDB;

LimitIterator::LimitIterator(hsql::LimitDescription* limit, TupleIterator* tupleIterator){
    this->limit = -1;
    if(limit->limit != nullptr && limit->limit->type == hsql::ExprType::kExprLiteralInt){
        this->limit = limit->limit->ival;
    }
    this->offset = 0;
    if(limit->offset != nullptr && limit->offset->type == hsql::ExprType::kExprLiteralInt){
        this->offset = limit->offset->ival;
    }
    this->tupleIterator = tupleIterator;
    this->index = -1;
}
LimitIterator::~LimitIterator(){
    delete tupleIterator;
}
Tuple* LimitIterator::fetchNext(){
    if(limit == -1 || index < limit){
        if(tupleIterator->hasNext()){
            auto tuple = tupleIterator->next();
            index += 1;
            return tuple;
        }else{
            return nullptr;
        }
    }else{
        return nullptr;
    }
}
void LimitIterator::open(){
    tupleIterator->open();
    for(int i = 0; i < offset && tupleIterator->hasNext(); i++){
        delete tupleIterator->next();
    }
    index = 0;
}
void LimitIterator::close(){
    tupleIterator->close();
    index = 0;
}
void LimitIterator::rewind(){
    tupleIterator->rewind();
    for(int i = 0; i < offset && tupleIterator->hasNext(); i++){
        delete tupleIterator->next();
    }
    index = 0;
}
TupleDesc* LimitIterator::getTupleDesc(){
    return tupleIterator->getTupleDesc();
}
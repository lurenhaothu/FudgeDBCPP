#include "WhereIterator.h"
#include "ExecutionUtility.h"

using namespace fudgeDB;

WhereIterator::WhereIterator(hsql::Expr *whereClause, TupleIterator* tupleIterator){
    this->whereClause = whereClause;
    this->tupleIterator = tupleIterator;
}

WhereIterator::~WhereIterator(){
    delete this->tupleIterator;
}

Tuple* WhereIterator::fetchNext(){
    while(tupleIterator->hasNext()){
        auto tuple = tupleIterator->next();
        if(ExecutionUtility::filter(tuple, this->whereClause)){
            return tuple;
        }else{
            delete tuple;
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
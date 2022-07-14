#include "OrderIterator.h"
#include "algorithm"
#include "ExecutionUtility.h"
#include "general/Tuple.h"
#include "iostream"

using namespace fudgeDB;

OrderIterator::OrderIterator(std::vector<hsql::OrderDescription*>* orders, TupleIterator* tupleIterator, std::unordered_map<std::string, hsql::Expr*>* aliasMap){
    this->orders = orders;
    this->tupleIterator = tupleIterator;
    this->index = -1;
    this->aliasMap = aliasMap;
}
OrderIterator::~OrderIterator(){
    delete tupleIterator;
}
Tuple* OrderIterator::fetchNext(){
    if(index != -1 && index < tuples.size()){
        index += 1;
        return tuples[index - 1];
    }else{
        return nullptr;
    }
}
void OrderIterator::open(){
    fetchAll();
    sort();
    index = 0;
}
void OrderIterator::close(){
    index = -1;
    tupleIterator->close();
}
void OrderIterator::rewind(){
    index = 0;
}
TupleDesc* OrderIterator::getTupleDesc(){
    return tupleIterator->getTupleDesc();
}
void OrderIterator::fetchAll(){
    tupleIterator->open();
    while(tupleIterator->hasNext()){
        tuples.push_back(new Tuple(tupleIterator->next(), tupleIterator->getTupleDesc()));
    }
}
void OrderIterator::sort(){
    std::sort(tuples.begin(), tuples.end(), [this](Tuple* tuple1, Tuple* tuple2){
        for(auto order : *this->orders){
            auto fieldExpr = order->expr;
            auto field1 = ExecutionUtility::getField(tuple1, fieldExpr, aliasMap);
            auto field2 = ExecutionUtility::getField(tuple2, fieldExpr, aliasMap);
            if(Field::filter(field1, field2, Op::equals)) continue;
            if(order->type == hsql::OrderType::kOrderDesc){
                return Field::filter(field1, field2, Op::greater);
            }else{
                return Field::filter(field1, field2, Op::less);
            }
        }
        return true;
    });
}
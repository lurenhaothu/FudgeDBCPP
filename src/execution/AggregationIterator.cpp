#include "AggregationIterator.h"
#include "general/Type.h"
#include "general/TupleDesc.h"
#include "general/Tuple.h"
#include "iostream"
#include "general/Field.h"
#include "ExecutionUtility.h"

using namespace fudgeDB;

Aggregation::Aggregation(AggregationFunc func){
    this->func = func;
    this->state = 0;
    this->count = 0;
}
void Aggregation::update(Field* field){
    int val;
    if(field->getType() == FieldType::stringField){
        if(func == AggregationFunc::COUNT) val = 1;
        else throw fudgeError("Aggregation Error: unsupported type");
    }else if(field->getType() == FieldType::intField){
        val = ((IntField*)field)->getValue();
    }else{
        throw fudgeError("Aggregation Error: unsupported type");
    }
    switch(func){
        case AggregationFunc::COUNT:
            count += 1;
            break;
        case AggregationFunc::MAX:
            if(count == 0) state = val;
            else state = val > state ? val : state;
            count += 1;
            break;
        case AggregationFunc::MIN:
            if(count == 0) state = val;
            else state = val < state ? val : state;
            count += 1;
            break;
        case AggregationFunc::SUM:
        case AggregationFunc::AVG:
            state += val;
            count += 1;
            break;
        default:
            throw fudgeError("Aggregation Error: unsupported type");
    }
}
Field* Aggregation::get(){
    switch(func){
        case AggregationFunc::COUNT:
            return new IntField(count);
        case AggregationFunc::MAX:
        case AggregationFunc::MIN:
        case AggregationFunc::SUM:
            return new IntField(state);
        case AggregationFunc::AVG:
            return new IntField(state / count);
        default:
            throw fudgeError("Aggregation Error: unsupported type");
    }
}

AggregationIterator::AggregationIterator(std::vector<hsql::Expr*> aggregates, 
    std::vector<hsql::Expr*> groupby, hsql::Expr* having, TupleIterator* tupleIterator, 
    std::unordered_map<std::string, hsql::Expr*>* aliasMap){
    
    //if(having != nullptr)std::cout<<having->type<<std::endl;
    this->aliasMap = aliasMap;

    this->tupleIterator = tupleIterator;
    this->childTupleDesc = tupleIterator->getTupleDesc();
    std::vector<std::string> colNames;
    std::vector<Type*> types;
    std::vector<std::string> tableNames;
    std::vector<std::string> alias;

    for(auto expr : aggregates){
        // aggregation column don't have table name or table alias name
        // because it may be the combination of multiplt tables
        if(expr->exprList == nullptr || expr->exprList->size() != 1 || expr->name == nullptr) 
            throw fudgeError("Aggregation error: unsupported aggregation function");
        auto aggrExpr = expr->exprList->at(0);
        this->aggregateColExpr.push_back(aggrExpr);
        auto aggrColNames = ExecutionUtility::getColName(expr, childTupleDesc, aliasMap);
        std::string funcName = expr->name;
        if(funcName == "COUNT" || funcName == "count"){
            aggregateFuncs.push_back(AggregationFunc::COUNT);
        }else if(funcName == "SUM" || funcName == "sum"){
            aggregateFuncs.push_back(AggregationFunc::SUM);
        }else if(funcName == "MAX" || funcName == "max"){
            aggregateFuncs.push_back(AggregationFunc::MAX);
        }else if(funcName == "MIN" || funcName == "min"){
            aggregateFuncs.push_back(AggregationFunc::MIN);
        }else if(funcName == "AVG" || funcName == "avg"){
            aggregateFuncs.push_back(AggregationFunc::AVG);
        }else{
            throw fudgeError("Agregation error: unsupported aggregation function");
        }
        colNames.push_back(aggrColNames[2]);
        types.push_back(new IntType());
        tableNames.push_back(aggrColNames[0]);
        alias.push_back(aggrColNames[1]);
    }
    for(auto expr : groupby){
        auto gbColNames = ExecutionUtility::getColName(expr, childTupleDesc, aliasMap);
        groupbyExpr.push_back(expr);
        colNames.push_back(gbColNames[2]);
        auto gbtype = ExecutionUtility::getColType(expr, childTupleDesc, aliasMap);
        types.push_back(gbtype);
        tableNames.push_back(gbColNames[0]);
        alias.push_back(gbColNames[1]);
    }
    auto temp = new TupleDesc(colNames, types, tableNames, alias);
    this->tupleDesc = new TupleDesc(temp, childTupleDesc);
    delete temp;
    this->having = having;
    tuples.clear();
    this->fetchTuples();
    index = -1;
    //std::cout<<"aggr size: "<<this->aggregateColExpr.size()<<std::endl;
}

AggregationIterator::~AggregationIterator(){
    delete this->tupleIterator;
    delete this->tupleDesc;
    for(auto& tuple : tuples){
        delete tuple;
    }
}
Tuple* AggregationIterator::fetchNext(){
    if(index >= 0 and index < tuples.size()){
        index += 1;
        return tuples[index - 1];
    }else{
        return nullptr;
    }
}
void AggregationIterator::open(){
    index = 0;
}
void AggregationIterator::close(){
    index = -1;
}
void AggregationIterator::rewind(){
    index = 0;
}
TupleDesc* AggregationIterator::getTupleDesc(){
    return this->tupleDesc;
}

void AggregationIterator::fetchTuples(){
    tupleIterator->open();
    std::unordered_map<std::string, std::vector<Aggregation>> aggrMap;
    std::unordered_map<std::string, std::vector<Field*>> keyMap;
    while(tupleIterator->hasNext()){
        auto tuple = tupleIterator->next();
        auto key = getKey(tuple);
        if(aggrMap.find(key) == aggrMap.end()){
            aggrMap[key] = std::vector<Aggregation>();
            keyMap[key] = std::vector<Field*>();
            for(auto aggrFunc : aggregateFuncs){
                aggrMap[key].push_back(Aggregation(aggrFunc));
            }
            for(auto gbExpr : groupbyExpr){
                keyMap[key].push_back(ExecutionUtility::getField(tuple, gbExpr, aliasMap));
            }
        }
        for(int i = 0; i < aggregateColExpr.size(); i++){
            auto newField = ExecutionUtility::getField(tuple, aggregateColExpr[i], aliasMap);
            aggrMap[key][i].update(newField);
            delete newField;
        }
        delete tuple;
    }
    tupleIterator->close();
    for(auto& itr : aggrMap){
        auto key = itr.first;
        std::vector<Field*> fields;
        for(auto& aggr : aggrMap[key]){
            fields.push_back(aggr.get());
        }
        for(auto& field : keyMap[key]){
            fields.push_back(Field::copy(field));
        }
        auto newTuple = new Tuple(this->tupleDesc, fields);
        //if(having != nullptr) std::cout<<"having type: "<<having->type<<std::endl;
        if(ExecutionUtility::filter(newTuple, having, this->aliasMap)){
            tuples.push_back(newTuple);
        }else{
            delete newTuple;
        }
    }
    for(auto& itr : keyMap){
        for(auto field : itr.second){
            delete field;
        }
    }
}

std::string AggregationIterator::getKey(Tuple* tuple){
    std::string key = "";
    for(auto expr : this->groupbyExpr){
        auto newField = ExecutionUtility::getField(tuple, expr, aliasMap);
        key += newField->toString();
        key += " ";
        delete newField;
    }
    return key;
}
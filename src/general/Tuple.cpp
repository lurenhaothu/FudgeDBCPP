#include "Tuple.h"
#include "Field.h"
#include "TupleDesc.h"
#include "Type.h"
#include <string>

using namespace fudgeDB;

Tuple::Tuple(TupleDesc* tupleDesc, std::vector<Field*> fields){
    this->tupleDesc = tupleDesc;
    this->fields = fields;
    this->nextAvail = this->PRESENT;
}

Tuple::Tuple(TupleDesc* tupleDesc, int nextAvail){
    this->nextAvail = nextAvail;
    this->tupleDesc = tupleDesc;
}

std::vector<Field*>* Tuple::getFields(){
    return &fields;
}
Field* Tuple::getField(int index){
    return fields[index];
}

int Tuple::getNextAvail(){
    return nextAvail;
}

Tuple::~Tuple(){
    for(auto field : fields){
        delete field;
    }
}

void Tuple::toString(char* tupleContent){
    int size = tupleDesc->getSize();
    int offset = sizeof(int);
    int* ptr = (int*)tupleContent;
    *ptr = nextAvail;
    if(nextAvail == Tuple::PRESENT){
        for(int i = 0; i < fields.size(); i++){
            fields[i]->toString(tupleContent + offset, tupleDesc->getType(i)->getSize());
            offset += tupleDesc->getType(i)->getSize();
        }
    }
}

Tuple* Tuple::parseTuple(TupleDesc* tupleDesc, char* tupleContent){
    int* ptr = (int*) tupleContent;
    int nextAvail = *(ptr);
    std::vector<Field*> fields;
    int offset = sizeof(int);
    for(int i = 0; i < tupleDesc->getLength(); i++){
        auto type = tupleDesc->getType(i);
        if(type->getType() == TypeEnum::IntTypeEnum){
            ptr = (int*)(tupleContent + offset);
            auto field = new IntField(*ptr);
            fields.push_back(field);
            offset += type->getSize();
        }else if(type->getType() == TypeEnum::StringTypeEnum){
            int len = type->getSize();
            std::string r;
            for(int j = 0; j < len; j++){
                if(*(tupleContent + offset + j) != '\0'){
                    r.push_back(*(tupleContent + offset + j));
                }else{
                    break;
                }
            }
            auto field = new StringField(r);
            fields.push_back(field);
            offset += type->getSize();
        }
    }
    if(nextAvail == Tuple::PRESENT){
        return new Tuple(tupleDesc, fields);
    }else{
        return new Tuple(tupleDesc, nextAvail);
    }
    
}

std::string Tuple::toString(){
    std::string res;
    for(int i = 0; i < fields.size(); i++){
        res += fields[i]->toString();
        if(i != fields.size() - 1) res += " | ";
    }
    return res;
}

Tuple::Tuple(Tuple* tuple1, Tuple* tuple2, TupleDesc* tupleDesc){
    for(auto field : *tuple1->getFields()){
        this->fields.push_back(Field::copy(field));
    }
    for(auto field : *tuple2->getFields()){
        this->fields.push_back(Field::copy(field));
    }
    this->tupleDesc = tupleDesc;
    this->nextAvail = this->PRESENT;
}
Tuple::Tuple(Tuple* tuple, TupleDesc* tupleDesc){
    for(auto field : *tuple->getFields()){
        this->fields.push_back(Field::copy(field));
    }
    this->tupleDesc = tupleDesc;
    this->nextAvail = this->PRESENT;
}
TupleDesc* Tuple::getTupleDesc(){
    return this->tupleDesc;
}
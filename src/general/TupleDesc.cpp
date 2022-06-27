#include "TupleDesc.h"
#include "Type.h"
#include "iostream"

using namespace fudgeDB;

TupleDesc::TupleDesc(std::vector<std::string> names, std::vector<Type*> types){
    //std::cout<<"tupleDesc constructor"<<std::endl;
    // length of names and types should be the same, but the check should be done in executor
    this->names = names;
    this->types = types; //class Type doesn't contain any pointer, so direct copy is okay
    this->size = sizeof(int);
    for(Type* t : types){
        size += t->getSize();
    }
}
TupleDesc::~TupleDesc(){
    //std::cout<<"tupleDesc destructor"<<std::endl;
    for(auto& t : types){
        delete t;
    }
}
int TupleDesc::getSize(){
    return size;
}
int TupleDesc::getLength(){
    return types.size();
}
std::vector<std::string>* TupleDesc::getNames(){
    return &names;
}
std::vector<Type*>* TupleDesc::getTypes(){
    return &types;
}
std::string TupleDesc::getName(int index){
    return names[index];
}
Type* TupleDesc::getType(int index){
    return types[index];
}
std::string TupleDesc::toString(){
    std::string res;
    for(int i = 0; i < names.size(); i++){
        res += names[i];
        switch(types[i]->getType()){
            case TypeEnum::IntTypeEnum:
                res += "(INT)";
                break;
            case TypeEnum::StringTypeEnum:
                res += "(STR " + std::to_string(types[i]->getSize()) + ")";
        }
        if(i != names.size() - 1){
            res += " | ";
        }
    }
    return res;
}
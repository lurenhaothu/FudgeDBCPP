#include "Field.h"
#include "fudgeDBError.h"

using namespace fudgeDB;

bool Field::filter(Field* field1, Field* field2, Op op){
    switch(op){
        case Op::equals:
            if(field1->getType() == field2->getType() && field1->toString() == field2->toString()){
                return true;
            }else{
                return false;
            }
            break;
        case Op::greater:
            if(field1->getType() != FieldType::intField || field2->getType() != FieldType::intField){
                throw fudgeError("unsupported compare type");
            }
            return ((IntField*)field1)->getValue() > ((IntField*)field2)->getValue();
        case Op::less:
            if(field1->getType() != FieldType::intField || field2->getType() != FieldType::intField){
                throw fudgeError("unsupported compare type");
            }
            return ((IntField*)field1)->getValue() < ((IntField*)field2)->getValue();
        default:
            throw fudgeError("unsupported compare operator");
    }
}
Field* Field::calculate(Field* field1, Field* field2, Op op){
    return nullptr;
    //TODO
}
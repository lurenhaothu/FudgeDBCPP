#include "Field.h"
#include "fudgeDBError.h"

using namespace fudgeDB;

Field* Field::copy(Field* field){
    switch(field->getType()){
        case FieldType::intField:
            return new IntField(*(IntField*)field);
        case FieldType::stringField:
            return new StringField(*(StringField*)field);
        default:
            throw fudgeError("Field copy failed, unsupported type");
    }
}

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
    if(field1->getType() != field2->getType()) throw fudgeError("Field type not matched for calculation");
    Field* res = nullptr;
    switch(op){
        case Op::plus:
            if(field1->getType() == FieldType::intField){
                return new IntField(((IntField*)field1)->getValue() + ((IntField*)field2)->getValue());
            }else if(field1->getType() == FieldType::stringField){
                return new StringField(((StringField*)field1)->getValue() + ((StringField*)field2)->getValue());
            }else{
                throw fudgeError("Unsupported calculate field type");
            }
        case Op::minus:
            if(field1->getType() == FieldType::intField){
                return new IntField(((IntField*)field1)->getValue() - ((IntField*)field2)->getValue());
            }else{
                throw fudgeError("Unsupported calculate field type");
            }
        case Op::times:
            if(field1->getType() == FieldType::intField){
                return new IntField(((IntField*)field1)->getValue() * ((IntField*)field2)->getValue());
            }else{
                throw fudgeError("Unsupported calculate field type");
            }
        case Op::divide:
            if(field1->getType() == FieldType::intField){
                if(((IntField*)field2)->getValue() == 0) throw fudgeError("divide by zero error");
                return new IntField(((IntField*)field1)->getValue() / ((IntField*)field2)->getValue());
            }else{
                throw fudgeError("Unsupported calculate field type");
            }
        default:
            throw fudgeError("Unsupported calculate type");
    }
}
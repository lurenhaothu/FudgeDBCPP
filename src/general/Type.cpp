#include "Type.h"

using namespace fudgeDB;

Type* Type::copy(Type* type){
    auto typeEnum = type->getType();
    if(typeEnum == TypeEnum::IntTypeEnum){
        return new IntType();
    }else if(typeEnum == TypeEnum::StringTypeEnum){
        return new StringType(type->getSize());
    }else{
        throw fudgeError("Type copy error");
    }
}
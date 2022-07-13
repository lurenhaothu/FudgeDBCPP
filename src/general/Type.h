#ifndef FUDGEDB_SRC_GENERAL_TYPE_H
#define FUDGEDB_SRC_GENERAL_TYPE_H

#include "fudgeDBError.h"

namespace fudgeDB{
    enum TypeEnum{
        IntTypeEnum,
        StringTypeEnum
    };
    class Type{
        private:
            int size;
        public:
            Type(int size){this->size = size;}
            int getSize(){return size;}
            virtual TypeEnum getType() = 0;
            virtual Type* makeCopy() = 0;
            static Type* copy(Type* type);
    };
    class IntType : public Type{
        public:
            IntType():Type(sizeof(int)){}
            TypeEnum getType(){return TypeEnum::IntTypeEnum;}
            Type* makeCopy(){return new IntType();}
    };
    class StringType : public Type{
        public:
            StringType(int length):Type(length){}
            TypeEnum getType(){return TypeEnum::StringTypeEnum;}
            Type* makeCopy(){return new StringType(this->getSize());}
    };
}

#endif
#ifndef FUDGEDB_SRC_GENERAL_TYPE_H
#define FUDGEDB_SRC_GENERAL_TYPE_H

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
    };
    class IntType : public Type{
        public:
            IntType():Type(sizeof(int)){}
            TypeEnum getType(){return TypeEnum::IntTypeEnum;}
    };
    class StringType : public Type{
        public:
            StringType(int length):Type(length){}
            TypeEnum getType(){return TypeEnum::StringTypeEnum;}
    };
}

#endif
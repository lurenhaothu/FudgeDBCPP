#ifndef FUDGEDB_SRC_GENERAL_FIELD_H
#define FUDGEDB_SRC_GENERAL_FIELD_H

#include <string>

namespace fudgeDB{
    enum FieldType{
        intField,
        stringField
    };
    enum Op{
        equals,
        less,
        greater,
        times,
        divide,
        plus,
        minus
    };
    class Field{
        private:
            //FieldType type;
        public:
            Field() = default;
            virtual ~Field() = default;
            virtual FieldType getType() = 0;
            virtual void toString(char*, int) = 0;
            virtual std::string toString() = 0;
            static bool filter(Field* field1, Field* field2, Op op); //TODO
            static Field* calculate(Field* field1, Field* field2, Op op); //TODO
    };
    class IntField : public Field{
        private:
            int val;
        public:
            IntField(int val){this->val = val;}
            int getValue(){return val;}
            FieldType getType(){return FieldType::intField;};
            void toString(char* addr, int len){
                int* ptr = (int*) addr;
                *ptr = val;
            }
            std::string toString(){
                return std::to_string(val);
            }
    };
    class StringField : public Field{
        private:
            std::string val;
        public:
            StringField(std::string val){this->val = val;}
            std::string getValue(){return val;}
            FieldType getType(){return FieldType::stringField;};
            void toString(char* addr, int len){
                for(int i = 0; i < len; i++){
                    if(i < val.size()) *(addr + i) = val[i];
                    else *(addr + i) = '\0';
                }
            }
            std::string toString(){
                return val;
            }
    };
}

#endif
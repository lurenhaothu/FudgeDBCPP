#ifndef FUDGEDB_SRC_GENERAL_TUPLEDESC_H
#define FUDGEDB_SRC_GENERAL_TUPLEDESC_H

#include <string>
#include <vector>

namespace fudgeDB{
    class Type;
    class TupleDesc{
        private:
            std::vector<std::string> names;
            std::vector<Type*> types;
            int size;
        public:
            TupleDesc(std::vector<std::string> names, std::vector<Type*> types);
            ~TupleDesc();
            int getSize(); //byte size of the tuple
            int getLength(); //col number
            std::vector<std::string>* getNames();
            std::vector<Type*>* getTypes();
            std::string getName(int index);
            Type* getType(int index);
    };
}

#endif
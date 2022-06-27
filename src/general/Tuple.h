#ifndef FUDGEDB_SRC_GENERAL_TUPLE_H
#define FUDGEDB_SRC_GENERAL_TUPLE_H

#include <vector>
#include <string>

namespace fudgeDB{
    class Field;
    class TupleDesc;

    class Tuple{
        public:
            static const int NONEXTAVAIL = -1;
            static const int PRESENT = -2;
        private:
            int nextAvail;
            std::vector<Field*> fields;
            TupleDesc* tupleDesc;
        public:
            Tuple(TupleDesc* tupleDesc, std::vector<Field*> fields);
            Tuple(TupleDesc* tupleDesc, int nextAvail); //make an empty tuple
            std::vector<Field*>* getFields();
            Field* getField(int index);
            int getNextAvail();
            ~Tuple();
            
            //toString for file io
            void toString(char*);
            static Tuple* parseTuple(TupleDesc* tupleDesc, char* tupleContent);

            //toString for CLI output
            std::string toString();
    };
}

#endif
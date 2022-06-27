#ifndef FUDGEDB_SRC_GENERAL_HEAPPAGE_H
#define FUDGEDB_SRC_GENERAL_HEAPPAGE_H

#include <vector>

namespace fudgeDB{
    class Tuple;
    class Table;
    class TupleIterator;
    class TupleDesc;

    class HeapPage{
        private:
            Table *table;
            int pageID; //the location of the page in the table file
            int tupleNum;
            std::vector<Tuple*> tuples;
        public:
            HeapPage(Table* table, int pageID, bool read); //file io
            ~HeapPage();
            int insertTuple(Tuple* tuple, int index);
            TupleIterator* getIterator();
            void flush(bool);

            std::vector<Tuple*>* getTuples();

            TupleDesc* getTupleDesc();
    };
}

#endif
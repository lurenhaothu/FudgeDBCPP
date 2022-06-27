#ifndef FUDGEDB_SRC_GENERAL_TABLEITERATOR_H
#define FUDGEDB_SRC_GENERAL_TABLEITERATOR_H

#include "TupleIterator.h"

namespace fudgeDB{
    class Table;
    class HeapPage;
    class HeapPageIterator;
    class TupleDesc;

    class TableIterator : public TupleIterator{
        private:
            Table* table;
            int pageID;
            HeapPage* page;
            TupleIterator* pageIterator;
        public:
            TableIterator(Table* table);
            ~TableIterator();
            Tuple* fetchNext();
            void open();
            void close();
            void rewind();
            TupleDesc* getTupleDesc();
    };
}

#endif
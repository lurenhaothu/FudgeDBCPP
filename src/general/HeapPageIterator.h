#ifndef FUDGEDB_SRC_GENERAL_HEAPPAGEITERATOR_H
#define FUDGEDB_SRC_GENERAL_HEAPPAGEITERATOR_H

#include "TupleIterator.h"

namespace fudgeDB{
    class HeapPage;

    class HeapPageIterator : public TupleIterator{
        private:
            HeapPage* page;
            int index;
        public:
            HeapPageIterator(HeapPage*);
            ~HeapPageIterator();
            Tuple* fetchNext();
            void open();
            void close();
            void rewind();
            TupleDesc* getTupleDesc();
    };
}

#endif
#ifndef FUDGEDB_SRC_EXECUTION_LIMITITERATOR_H
#define FUDGEDB_SRC_EXECUTION_LIMITITERATOR_H

#include "general/TupleIterator.h"
#include "SQLParser.h"
#include "general/Tuple.h"

namespace fudgeDB{
    class LimitIterator : public TupleIterator{
        private:
            int offset;
            int limit;
            TupleIterator* tupleIterator;
            int index;
        public:
            LimitIterator(hsql::LimitDescription* limit, TupleIterator* tupleIterator);
            ~LimitIterator();
            Tuple* fetchNext();
            void open();
            void close();
            void rewind();
            TupleDesc* getTupleDesc();
    };
}


#endif
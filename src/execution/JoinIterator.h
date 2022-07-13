#ifndef FUDGEDB_SRC_EXECUTION_JOINITERATOR_H
#define FUDGEDB_SRC_EXECUTION_JOINITERATOR_H

#include "SQLParser.h"
#include "general/TupleIterator.h"

namespace fudgeDB{
    class Tuple;

    class JoinIterator : public TupleIterator{
        private:
            TupleIterator* leftItr;
            TupleIterator* rightItr;
            hsql::Expr* condition;
            TupleDesc* tupleDesc;
            Tuple* leftTuple;
        public:
            JoinIterator(TupleIterator* leftItr, TupleIterator* rightItr, hsql::Expr* condition);
            JoinIterator(std::vector<TupleIterator*>* tupleIterators);
            ~JoinIterator();
            Tuple* fetchNext();
            void open();
            void close();
            void rewind();
            TupleDesc* getTupleDesc();
        private:
            Tuple* fetchWithoutFilter();
    };
}

#endif
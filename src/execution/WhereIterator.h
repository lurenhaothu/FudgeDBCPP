#ifndef FUDGEDB_SRC_EXECUTION_WHEREITERATOR_H
#define FUDGEDB_SRC_EXECUTION_WHEREITERATOR_H

#include "general/Field.h"
#include "general/TupleIterator.h"
#include "SQLParser.h"

namespace fudgeDB{
    class WhereIterator : public TupleIterator{
        private:
            hsql::Expr *whereClause;
            TupleIterator* tupleIterator;
        public:
            WhereIterator(hsql::Expr *whereClause, TupleIterator* tupleIterator);
            ~WhereIterator();
            Tuple* fetchNext();
            void open();
            void close();
            void rewind();
            TupleDesc* getTupleDesc();
    };
}

#endif
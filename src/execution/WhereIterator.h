#ifndef FUDGEDB_SRC_EXECUTION_WHEREITERATOR_H
#define FUDGEDB_SRC_EXECUTION_WHEREITERATOR_H

#include "general/TupleIterator.h"
#include "SQLParser.h"

namespace fudgeDB{
    class WhereIterator : public TupleIterator{
        private:
            hsql::Expr *whereClause;
            TupleIterator* tupleIterator;
        public:
            WhereIterator(hsql::Expr *whereClause, TupleIterator* tupleIterator);
            ~WhereIterator() = default;
            Tuple* fetchNext();
            void open();
            void close();
            void rewind();
            TupleDesc* getTupleDesc();
        private:
            bool filter(Tuple* tuple, hsql::Expr* expr);
            bool binaryFilter(Tuple* tuple, hsql::Expr* expr, Op op);
            Field* getField(Tuple* tuple, hsql::Expr* expr);
            Field* getFieldByColumnRef(Tuple* tuple, hsql::Expr* expr);
            Field* getFieldByCalculation(Tuple* tuple, hsql::Expr* expr);
    };
}

#endif
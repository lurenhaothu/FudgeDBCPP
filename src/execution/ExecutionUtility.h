#ifndef FUDGEDB_SRC_EXECUTION_EXECUTIONUTILITY_H
#define FUDGEDB_SRC_EXECUTION_EXECUTIONUTILITY_H

#include "general/Field.h"

namespace hsql{
    class Expr;
}

namespace fudgeDB{
    class Tuple;
    class ExecutionUtility{
        public:
            static bool filter(Tuple* tuple, hsql::Expr* expr);
            static bool binaryFilter(Tuple* tuple, hsql::Expr* expr, Op op);
            static Field* getField(Tuple* tuple, hsql::Expr* expr);
            static Field* getFieldByColumnRef(Tuple* tuple, hsql::Expr* expr);
            static Field* getFieldByCalculation(Tuple* tuple, hsql::Expr* expr);
    };
}

#endif
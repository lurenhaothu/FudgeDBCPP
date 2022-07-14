#ifndef FUDGEDB_SRC_EXECUTION_EXECUTIONUTILITY_H
#define FUDGEDB_SRC_EXECUTION_EXECUTIONUTILITY_H

#include "general/Field.h"
#include "vector"
#include "unordered_map"

namespace hsql{
    class Expr;
}

namespace fudgeDB{
    class Tuple;
    class TupleDesc;
    class Type;
    class ExecutionUtility{
        public:
            static bool filter(Tuple* tuple, hsql::Expr* expr, std::unordered_map<std::string, hsql::Expr*>* aliasMap);
            static bool binaryFilter(Tuple* tuple, hsql::Expr* expr, Op op, std::unordered_map<std::string, hsql::Expr*>* aliasMap);
            static Field* getField(Tuple* tuple, hsql::Expr* expr, std::unordered_map<std::string, hsql::Expr*>* aliasMap);
            static Field* getFieldByColumnRef(Tuple* tuple, hsql::Expr* expr, std::unordered_map<std::string, hsql::Expr*>* aliasMap);
            static Field* getFieldByAggregate(Tuple* tuple, hsql::Expr* expr, std::unordered_map<std::string, hsql::Expr*>* aliasMap);
            static Field* getFieldByCalculation(Tuple* tuple, hsql::Expr* expr, std::unordered_map<std::string, hsql::Expr*>* aliasMap);
            //return table name, table alias, and Colname in turn
            static std::vector<std::string> getColName(hsql::Expr* expr, TupleDesc* tupleDesc, std::unordered_map<std::string, hsql::Expr*>* aliasMap); 
            //get all functionRef expr from the tree
            static void getAggregationExpr(std::vector<hsql::Expr*>* list, 
                std::vector<hsql::Expr*>& aggrList);
            static void getAggregationExpr(hsql::Expr* expr, 
                std::vector<hsql::Expr*>& aggrList);
            static std::unordered_map<std::string, hsql::Expr*>* getColAliasMap
                (std::vector<hsql::Expr*>* selectList);
            static Type* getColType(hsql::Expr* expr, TupleDesc* tupleDesc, std::unordered_map<std::string, hsql::Expr*>* aliasMap); 
    };
}

#endif
#ifndef FUDGEDB_SRC_EXECUTION_AGGREGATIONITERATOR_H
#define FUDGEDB_SRC_EXECUTION_AGGREGATIONITERATOR_H

#include "general/TupleIterator.h"
#include "SQLParser.h"
#include "unordered_map"

namespace fudgeDB{
    class Field;

    enum AggregationFunc{
        COUNT,
        SUM,
        MIN,
        MAX,
        AVG
    };
    class Aggregation{
        private:
            AggregationFunc func;
            int state;
            int count;
        public:
            Aggregation(AggregationFunc func);
            ~Aggregation() = default;
            void update(Field* field);
            Field* get();
    };
    class AggregationIterator : public TupleIterator{
        private:
            TupleIterator* tupleIterator;
            TupleDesc* tupleDesc;
            TupleDesc* childTupleDesc;
            std::vector<hsql::Expr*> aggregateColExpr;
            std::vector<AggregationFunc> aggregateFuncs;
            std::vector<hsql::Expr*> groupbyExpr;
            std::vector<Tuple*> tuples;
            int index;
            hsql::Expr* having;
            std::unordered_map<std::string, hsql::Expr*>* aliasMap;
        public:
            AggregationIterator(std::vector<hsql::Expr*> aggregates, std::vector<hsql::Expr*> groupby, hsql::Expr* having, 
                TupleIterator* tupleIterator, std::unordered_map<std::string, hsql::Expr*>* aliasMap);
            ~AggregationIterator();
            Tuple* fetchNext();
            void open();
            void close();
            void rewind();
            TupleDesc* getTupleDesc();

        private:
            void fetchTuples();
            std::string getKey(Tuple* tuple);
    };
}

#endif
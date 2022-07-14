#ifndef FUDGEDB_SRC_EXECUTION_ORDERITERATOR_H
#define FUDGEDB_SRC_EXECUTION_ORDERITERATOR_H

#include "general/TupleIterator.h"
#include "SQLParser.h"
#include "unordered_map"

namespace fudgeDB{
    class OrderIterator : public TupleIterator{
        private:
            TupleIterator* tupleIterator;
            std::vector<Tuple*> tuples;
            std::vector<hsql::OrderDescription*>* orders;
            std::unordered_map<std::string, hsql::Expr*>* aliasMap;
            int index;
        public:
            OrderIterator(std::vector<hsql::OrderDescription*>* orders, TupleIterator* tupleIterator, 
                std::unordered_map<std::string, hsql::Expr*>* aliasMap);
            ~OrderIterator();
            Tuple* fetchNext();
            void open();
            void close();
            void rewind();
            TupleDesc* getTupleDesc();
        private:
            void fetchAll();
            void sort();
    };
}

#endif
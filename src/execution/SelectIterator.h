#ifndef FUDGEDB_SRC_EXECUTION_SELECTITERATOR_H
#define FUDGEDB_SRC_EXECUTION_SELECTITERATOR_H

#include "general/TupleIterator.h"
#include "SQLParser.h"
#include "unordered_map"
#include "vector"

namespace fudgeDB{
    class TupleDesc;
    class SelectIterator : public TupleIterator{
        private:
            TupleIterator* tupleIterator;
            std::vector<hsql::Expr*>* selectList;
            std::unordered_map<std::string, hsql::Expr*>* aliasMap;
            TupleDesc* tupleDesc;
            TupleDesc* childTupleDesc;
        public:
            SelectIterator(std::vector<hsql::Expr*>* selectList, TupleIterator* tupleIterator, 
                std::unordered_map<std::string, hsql::Expr*>* aliasMap);
            ~SelectIterator();
            Tuple* fetchNext();
            void open();
            void close();
            void rewind();
            TupleDesc* getTupleDesc();
        private:
            Tuple* getTuple(Tuple* tuple);
    };
}

#endif
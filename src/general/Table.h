#ifndef FUDGEDB_SRC_GENERAL_TABLE_H
#define FUDGEDB_SRC_GENERAL_TABLE_H

#include <string>

namespace fudgeDB{
    class TupleDesc;
    class Tuple;
    class TupleIterator;
    class Table{
        private:
            std::string name;
            TupleDesc* tupleDesc;
            int pageNum;
            int availHead;
        public:
            Table(std::string name, TupleDesc* tupleDesc, int pageNum, int availHead);
            ~Table();
            TupleDesc* getTupleDesc();
            std::string getName();
            int getPageNum();
            void insertTuple(Tuple* tuple);
            void addPage();
            TupleIterator* getIterator();

            static Table* parseTable(std::string str);
            std::string toString();
        private:
            void appendPage();
    };
}

#endif
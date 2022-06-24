#ifndef FUDGEDB_SRC_GENERAL_TUPLEITERATOR_H
#define FUDGEDB_SRC_GENERAL_TUPLEITERATOR_H

#include "fudgeDBError.h"

namespace fudgeDB{
    class Tuple;

    class TupleIterator{
        private:
            Tuple* nextTuple;
        public:
            TupleIterator(){nextTuple = nullptr;}
            virtual Tuple* fetchNext() = 0;
            Tuple* next(){
                if(nextTuple == nullptr){
                    nextTuple = fetchNext();
                    if(nextTuple == nullptr){
                        throw fudgeError("TupleIterator next error");
                    }
                }
                Tuple* result = nextTuple;
                nextTuple = nullptr;
                return result;  
            }
            bool hasNext(){
                if(nextTuple == nullptr) nextTuple = fetchNext();
                return nextTuple != nullptr;
            }
            virtual void open() = 0;
            virtual void close() = 0;
            virtual void rewind() = 0;
    };
}

#endif
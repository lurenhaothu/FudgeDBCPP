#include "HeapPageIterator.h"
#include "HeapPage.h"
#include "Tuple.h"
#include "FudgeDB.h"
#include "MemBuffer.h"

using namespace fudgeDB;

HeapPageIterator::HeapPageIterator(HeapPage* page){
    this->page = page;
    index = -1;
}
Tuple* HeapPageIterator::fetchNext(){
    if(index == -1) return nullptr;
    while(index < page->getTuples()->size() 
        && page->getTuples()->at(index)->getNextAvail() != Tuple::PRESENT){
        index ++;
    }
    if(index < page->getTuples()->size()){
        auto temp = page->getTuples()->at(index);
        index ++;
        return temp;
    }else{
        return nullptr;
    }
}
void HeapPageIterator::open(){
    index = 0;
}
void HeapPageIterator::close(){
    index = -1;
}
void HeapPageIterator::rewind(){
    index = 0;
}
HeapPageIterator::~HeapPageIterator(){
    //release page for MemBuffer
    // no need to do this anymore
    // page release done by table iterator
}

TupleDesc* HeapPageIterator::getTupleDesc(){
    return page->getTupleDesc();
}
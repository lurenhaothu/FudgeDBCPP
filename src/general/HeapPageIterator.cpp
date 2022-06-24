#include "HeapPageIterator.h"
#include "HeapPage.h"
#include "Tuple.h"

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
    //TODO: release page for MemBuffer
}
#include "HeapPage.h"
#include "Table.h"
#include "TupleDesc.h"
#include "MemBuffer.h"
#include <fstream>
#include "Tuple.h"
#include "iostream"
#include "HeapPageIterator.h"
#include "fudgeDBError.h"

using namespace fudgeDB;

HeapPage::HeapPage(Table* table, int pageID, bool read){
    this->table = table;
    this->pageID = pageID;
    auto tupleDesc = table->getTupleDesc();
    int size = tupleDesc->getSize();
    this->tupleNum = MemBuffer::PAGESIZE / size;
    if(read){
        std::fstream file;
        file.open(table->getName() + ".table", std::fstream::in | std::fstream::out);
        if(file.is_open()){
            file.seekg(pageID * MemBuffer::PAGESIZE);
            char* pageContent = new char[MemBuffer::PAGESIZE];
            file.read(pageContent, MemBuffer::PAGESIZE);
            for(int i = 0; i < MemBuffer::PAGESIZE; i += size){
                auto newTuple = Tuple::parseTuple(tupleDesc, pageContent + i);
                tuples.push_back(newTuple);
            }
            delete[] pageContent;
            file.close();
        }else{
            throw fudgeError("File IO error");
        }
    }else{
        for(int i = 0; i < tupleNum; i++){
            if(i != tupleNum - 1){
                tuples.push_back(new Tuple(tupleDesc, pageID * tupleNum + i + 1));
            }else{
                tuples.push_back(new Tuple(tupleDesc, -1));
            }
        }
    }
}
HeapPage::~HeapPage(){
    for(auto tuple : tuples){
        if(tuple != nullptr){
            delete tuple;
        }
    }
}
int HeapPage::insertTuple(Tuple* tuple, int index){
    int availHead = tuples[index]->getNextAvail();
    if(tuples[index] != nullptr){
        delete tuples[index];
    }
    tuples[index] = tuple;
    return availHead;
}
TupleIterator* HeapPage::getIterator(){
    return new HeapPageIterator(this);
}
void HeapPage::flush(bool append){
    char* pageContent = new char[MemBuffer::PAGESIZE];
    int size = this->table->getTupleDesc()->getSize();
    //std::cout<<size<<std::endl;
    for(int i = 0; i < this->tupleNum; i++){
        //std::cout<<i<<std::endl;
        tuples[i]->toString(pageContent + i * size);
    }
    std::fstream file;
    //std::cout<<"prepare open"<<std::endl;
    if(append){
        file.open(table->getName() + ".table", std::fstream::out | std::fstream::app);
    }else{
        file.open(table->getName() + ".table", std::fstream::out | std::fstream::in);
    }
    if(file.is_open()){
        //std::cout<<"file is open"<<std::endl;
        file.seekg(pageID * MemBuffer::PAGESIZE);
        file.write(pageContent, MemBuffer::PAGESIZE);
        file.close();
    }else{
        throw fudgeError("File IO error");
    }
    delete[] pageContent;
}

std::vector<Tuple*>* HeapPage::getTuples(){
    return  &tuples;
}

TupleDesc* HeapPage::getTupleDesc(){
    return table->getTupleDesc();
}
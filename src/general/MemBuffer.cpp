#include "MemBuffer.h"
#include "HeapPage.h"
#include "Table.h"

using namespace fudgeDB;

MemBuffer::MemBuffer(){
    this->head = LinkedNode();
    this->head = LinkedNode();
    head.next = &tail;
    tail.prev = &head;
}

HeapPage* MemBuffer::getPage(Table* table, int pageID, bool write){
    if(pageMap.size() >= BUFFERSIZE){
        evictPage();
    }
    std::string key = table->getName() + " " + std::to_string(pageID);
    if(pageMap.find(key) == pageMap.end()){
        auto page = new HeapPage(table, pageID, true);
        pageMap[key] = new LinkedNode(page, key, write);
        pinned.push_back(key);
        return page;
    }else{
        auto node = pageMap[key];
        node->write |= write;
        node->pop();
        return node->page;
    }
}

void MemBuffer::releasePage(Table* table, int pageID){
    std::string key = table->getName() + " " + std::to_string(pageID);
    auto node = pageMap[key];
    node->insert(&head);
}

void MemBuffer::evictPage(){
    auto node = this->tail.prev;
    if(node->write) node->page->flush(false);
    node->pop();
    pageMap.erase(node->key);
}

void MemBuffer::flushAll(){
    for(auto& itr : pageMap){
        if(itr.second->write)itr.second->page->flush(false);
    }
}

MemBuffer::~MemBuffer(){
    for(auto& itr : pageMap){
        delete itr.second;
    }
}
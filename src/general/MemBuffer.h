#ifndef FUDGEDB_SRC_GENERAL_MEMBUFFER_H
#define FUDGEDB_SRC_GENERAL_MEMBUFFER_H

#include <unordered_map>
#include <deque>
#include <vector>
#include <utility> 
#include "HeapPage.h"
#include "iostream"

namespace fudgeDB{
    class HeapPage;
    class Table;
    class LinkedNode{
        public:
            LinkedNode* prev;
            LinkedNode* next;
            std::string key;
            HeapPage* page;
            bool write;
            LinkedNode(HeapPage* page, std::string key, bool write){
                this->page = page;
                this->write = write;
                this->key = key;
            }
            ~LinkedNode(){
                if(page != nullptr){
                    delete page;
                }
            }
            LinkedNode() = default;
            void pop(){
                if(prev != nullptr) prev->next = next;
                if(next != nullptr) next->prev = prev;
                prev = nullptr;
                next = nullptr;
            }
            void insert(LinkedNode* front){
                prev = front;
                next = front->next;
                prev->next = this;
                next->prev = this;
            }
    };
    class MemBuffer{
        public:
            static const int PAGESIZE = 4096;
            static const int BUFFERSIZE = 50;
        private:
            std::unordered_map<std::string, LinkedNode*> pageMap;
            std::vector<std::string> pinned;
            LinkedNode head;
            LinkedNode tail;

        public:
            MemBuffer();
            ~MemBuffer();
            void flushAll();
            HeapPage* getPage(Table* table, int pageID, bool write);
            void releasePage(Table* table, int pageID);
        private:
            void evictPage();
    };
}

#endif

/*MemBuffer
保留所有dirty pages
按照MRU, most recently used, 从buffer移除
只有写需要标transactionID,读可以共享
如果已经有人读，求写，block
如果有人写，求读， block
同一transaction，读变读，写变写，直接返回，读变写，写变读，都直接返回。假设同一个transaction不会同时读多个页
记录所有pinned pages
问题：何时算写完了？何时释放？
*/
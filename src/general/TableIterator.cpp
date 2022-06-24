#include "TableIterator.h"
#include "FudgeDB.h"
#include "MemBuffer.h"
#include "Table.h"
#include "HeapPage.h"
#include "HeapPageIterator.h"
#include "TupleIterator.h"

using namespace fudgeDB;

TableIterator::TableIterator(Table* table){
    this->table = table;
    pageID = -1;
    page = nullptr;
    pageIterator = nullptr;
}
TableIterator::~TableIterator(){
    if(page != nullptr)FudgeDB::getFudgDB()->getMemBuffer()->releasePage(table, pageID);
    pageID = -1;
    page = nullptr;
    delete pageIterator;
}
Tuple* TableIterator::fetchNext(){
    if(pageID == -1) return nullptr;
    if(pageIterator == nullptr){
        while(pageID < table->getPageNum()){
            page = FudgeDB::getFudgDB()->getMemBuffer()->getPage(table, pageID, false);
            pageIterator = page->getIterator();
            pageIterator->open();
            if(!pageIterator->hasNext()){
                pageIterator->close();
                delete pageIterator;
                FudgeDB::getFudgDB()->getMemBuffer()->releasePage(table, pageID);
                page = nullptr;
                pageID++;
            }else{
                return pageIterator->next();
            }
        }
        return nullptr;
    }else{
        if(pageIterator->hasNext()){
            return pageIterator->next();
        }else{
            FudgeDB::getFudgDB()->getMemBuffer()->releasePage(table, pageID);
            delete pageIterator;
            page = nullptr;
            pageID++;
            while(pageID < table->getPageNum()){
                page = FudgeDB::getFudgDB()->getMemBuffer()->getPage(table, pageID, false);
                pageIterator = page->getIterator();
                pageIterator->open();
                if(!pageIterator->hasNext()){
                    pageIterator->close();
                    delete pageIterator;
                    FudgeDB::getFudgDB()->getMemBuffer()->releasePage(table, pageID);
                    page = nullptr;
                    pageID++;
                }else{
                    return pageIterator->next();
                }
            }
            return nullptr;
        }
    }
}
void TableIterator::open(){
    pageID = 0;
    page = nullptr;
    pageIterator = nullptr;
}
void TableIterator::close(){
    if(page != nullptr)FudgeDB::getFudgDB()->getMemBuffer()->releasePage(table, pageID);
    pageID = -1;
    page = nullptr;
    pageIterator = nullptr;
}
void TableIterator::rewind(){
    if(page != nullptr)FudgeDB::getFudgDB()->getMemBuffer()->releasePage(table, pageID);
    pageID = 0;
    page = nullptr;
    pageIterator = nullptr;
}
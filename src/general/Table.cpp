#include "Table.h"
#include "TupleDesc.h"
#include "sstream"
#include "Type.h"
#include "HeapPage.h"
#include "MemBuffer.h"
#include "FudgeDB.h"
#include "TableCatalog.h"
#include "iostream"
#include "TableIterator.h"
#include "Tuple.h"

using namespace fudgeDB;

Table::Table(std::string name, TupleDesc* tupleDesc, int pageNum, int availHead){
    this->name = name;
    this->tupleDesc = tupleDesc; //tupleDesc is dynamically allocated by new;
    this->availHead = availHead;
    this->pageNum = pageNum;
}
Table::~Table(){
    if(this->tupleDesc != nullptr){
        delete tupleDesc;
    }
}
TupleDesc* Table::getTupleDesc(){
    return this->tupleDesc;
}
std::string Table::getName(){
    return this->name;
}
void Table::insertTuple(Tuple* tuple){
    if(availHead == Tuple::NONEXTAVAIL){
        this->addPage();
    }
    int pageID = availHead / (MemBuffer::PAGESIZE / tupleDesc->getSize());
    int offset = availHead % (MemBuffer::PAGESIZE / tupleDesc->getSize());
    auto page = FudgeDB::getFudgDB()->getMemBuffer()->getPage(this, pageID, true);
    availHead = page->insertTuple(tuple, offset);
    FudgeDB::getFudgDB()->getMemBuffer()->releasePage(this, pageID);
    //FudgeDB::getFudgDB()->getTableCatalog()->updateCatalog();
}

void Table::addPage(){
    auto page = new HeapPage(this, pageNum, false);
    availHead = pageNum * (MemBuffer::PAGESIZE / tupleDesc->getSize());
    pageNum++;
    page->flush(true);
    delete page;
    //FudgeDB::getFudgDB()->getTableCatalog()->updateCatalog();
}

Table* Table::parseTable(std::string str){
    std::string name;
    int pageNum;
    int availHead;
    std::vector<std::string> names;
    std::vector<Type*> types;
    int n;
    std::istringstream ss(str);
    ss >> n;
    ss >> name;
    ss >> pageNum;
    ss >> availHead;
    std::string colname;
    std::string coltype;
    int len;
    for(int i = 0; i < n; i++){
        ss >> colname;
        ss >> coltype;
        ss >> len;
        if(coltype == "INT"){
            names.push_back(colname);
            types.push_back(new IntType());
        }else if(coltype == "STR"){
            names.push_back(colname);
            types.push_back(new StringType(len));
        }
    }
    return new Table(name, new TupleDesc(names, types), pageNum, availHead);
}

std::string Table::toString(){
    std::string res;
    int n = getTupleDesc()->getLength();
    res += std::to_string(n);
    res += " ";
    res += getName();
    res += " ";
    res += std::to_string(pageNum);
    res += " ";
    res += std::to_string(availHead);
    res += " ";
    auto tupleDesc = getTupleDesc();
    for(int i = 0; i < n; i++){
        res += tupleDesc->getName(i);
        res += " ";
        if(tupleDesc->getType(i)->getType() == TypeEnum::IntTypeEnum){
            res += "INT ";
            res += std::to_string(tupleDesc->getType(i)->getSize());
            res += " ";
        }else if(tupleDesc->getType(i)->getType() == TypeEnum::StringTypeEnum){
            res += "STR ";
            res += std::to_string(tupleDesc->getType(i)->getSize());
            res += " ";
        }
    }
    return res;
}

int Table::getPageNum(){
    return this->pageNum;
}

TupleIterator* Table::getIterator(){
    return new TableIterator(this);
}
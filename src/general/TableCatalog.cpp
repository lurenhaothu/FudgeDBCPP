#include "TableCatalog.h"
#include "fstream"
#include "Table.h"
#include "TupleDesc.h"
#include "Type.h"
#include "sstream"
#include "iostream"
#include "fudgeDBError.h"

using namespace fudgeDB;

TableCatalog::TableCatalog(std::string CatalogFile){
    //load table catalog from file
    this->CatalogFile = CatalogFile;
    std::fstream file;
    file.open(CatalogFile);
    if(file.is_open()){
        std::string line;
        while(std::getline(file, line)){
            Table* table = Table::parseTable(line);
            this->tableMap[table->getName()] = table;
        }
        file.close();
    }else{
        //throw fudgeError("File IO error");
        // no nothing
        //new DB, no previous table catalog exists
    }
}
TableCatalog::~TableCatalog(){
    for(auto& itr : tableMap){
        if(itr.second != nullptr){
            delete itr.second;
        }
    }
}
void TableCatalog::insertTable(std::string name, Table* table){
    if(tableMap.find(name) == tableMap.end()){
        tableMap[name] = table;
        this->updateCatalog();
    }
}

Table* TableCatalog::getTable(std::string name){
    if(tableMap.find(name) == tableMap.end()){
        return nullptr;
    }else{
        return tableMap[name];
    }
}
void TableCatalog::updateCatalog(){
    std::fstream file;
    file.open(CatalogFile, std::fstream::out | std::fstream::trunc);
    if(file.is_open()){
        //std::cout<<"file is open"<<std::endl;
        for(auto& itr : tableMap){
            file << itr.second->toString() << std::endl;
        }
        file.close();
    }else{
        throw fudgeError("File IO error");
    }
}
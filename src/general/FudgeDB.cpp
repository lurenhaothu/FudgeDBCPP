#include "FudgeDB.h"
#include "TableCatalog.h"
#include "MemBuffer.h"
#include "SQLParser.h"
#include "Executor.h"
#include "fudgeDBError.h"

using namespace fudgeDB;

FudgeDB* FudgeDB::singletonFudgeDB;

FudgeDB::FudgeDB(std::string DBPath){
    this->DBPath = DBPath;
}
FudgeDB* FudgeDB::getFudgDB(std::string DBPath){
    if(singletonFudgeDB == nullptr){
        singletonFudgeDB = new FudgeDB(DBPath);
        return singletonFudgeDB;
    }else{
        return singletonFudgeDB; //if singletonFudgeDB is not null, you should call getFudgeDB();
    }
}
FudgeDB* FudgeDB::getFudgDB(){
    return singletonFudgeDB;
}
FudgeDB::~FudgeDB(){
    delete singletonFudgeDB;
}
TableCatalog* FudgeDB::getTableCatalog(){
    return this->tableCatalog;
}
MemBuffer* FudgeDB::getMemBuffer(){
    return this->memBuffer;
}
void FudgeDB::open(){
    tableCatalog =  new TableCatalog(this->DBPath + this->tableCatalogName); 
    this->memBuffer = new MemBuffer();
    this->executor = new Executor();
}
void FudgeDB::close(){
    FudgeDB::getFudgDB()->getTableCatalog()->updateCatalog();
    memBuffer->flushAll();
    if(memBuffer != nullptr) delete memBuffer;
    if(tableCatalog != nullptr) delete tableCatalog;
    if(executor != nullptr) delete executor;
}
void FudgeDB::run(){
    std::string query;
    bool exit = false;
    while(!exit){
        std::cout<<"FudgeDB>";
        std::getline(std::cin, query);
        if(query == "exit"){
            exit = true;
            break;
        }
        hsql::SQLParserResult result;
        hsql::SQLParser::parse(query, &result);
        if (result.isValid()) {
            for (auto i = 0; i < result.size(); ++i) {
                try{
                    std::cout<<executor->executeStatement(result.getStatement(i))<<std::endl;
                }catch(fudgeError err){
                    std::cout<<"execution error: "<<err.what()<<std::endl;
                }
            }
        }else{
            std::cout<<"SQL Parse error"<<std::endl;
        }
    }
}
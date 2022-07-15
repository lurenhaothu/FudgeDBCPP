#include "FudgeDB.h"
#include "TableCatalog.h"
#include "MemBuffer.h"
#include "SQLParser.h"
#include "Executor.h"
#include "fudgeDBError.h"
#include <readline/readline.h>
#include <readline/history.h>


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
    if(tableCatalog != nullptr) tableCatalog->updateCatalog();
    if(memBuffer != nullptr) memBuffer->flushAll();
    delete memBuffer;
    delete tableCatalog;
    delete executor;
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
    delete memBuffer;
    delete tableCatalog;
    delete executor;
}
void FudgeDB::run(){
    bool exit = false;
    while(!exit){
        char * line = readline("FudgeDB > ");
        if(!line) break;
        if(*line) add_history(line);
        std::string query = line;
        if(query == "exit"){
            exit = true;
            continue;
        }//else if(query.size() == 0){
            //continue;
        //}
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
        free(line);
    }
}

void FudgeDB::run(std::fstream& cmdFile, std::fstream& output){
    std::string query;
    while(std::getline(cmdFile, query)){
        hsql::SQLParserResult result;
        hsql::SQLParser::parse(query, &result);
        if (result.isValid()) {
            for (auto i = 0; i < result.size(); ++i) {
                try{
                    output<<executor->executeStatement(result.getStatement(i))<<std::endl;
                }catch(fudgeError err){
                    output<<"execution error: "<<err.what()<<std::endl;
                }
            }
        }else{
            output<<"SQL Parse error"<<std::endl;
        }
    }
}
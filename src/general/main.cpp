#include "FudgeDB.h"

#include <string>
#include "iostream"
#include "fstream"

using namespace fudgeDB;

int main(int argc, char** argv){
    if(argc == 1){
        FudgeDB *fudgeDB = FudgeDB::getFudgDB("");
        fudgeDB->open();
        fudgeDB->run();
        fudgeDB->close();
        return 0;
    }else if(argc == 2){
        std::fstream cmdFile(argv[1], std::fstream::in);
        std::fstream output("FudgeDBOutput.txt", std::fstream::out);
        if(cmdFile.is_open()){
            FudgeDB *fudgeDB = FudgeDB::getFudgDB("");
            fudgeDB->open();
            fudgeDB->run(cmdFile, output);
            fudgeDB->close();
            cmdFile.close();
            output.close();
        }else{
            std::cout<<"Could not open cmd File"<<std::endl;
        }
        return 0;
    }else if(argc == 3){
        std::fstream cmdFile(argv[1], std::fstream::in);
        std::fstream output(argv[2], std::fstream::out);
        if(cmdFile.is_open()){
            FudgeDB *fudgeDB = FudgeDB::getFudgDB("");
            fudgeDB->open();
            fudgeDB->run(cmdFile, output);
            fudgeDB->close();
            cmdFile.close();
            output.close();
        }else{
            std::cout<<"Could not open cmd File"<<std::endl;
        }
        return 0;
    }else{
        std::cout<<"Too many arguments"<<std::endl;
        return 0;
    }
    
}
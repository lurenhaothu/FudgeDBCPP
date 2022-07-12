#include "FudgeDB.h"

#include <string>

using namespace fudgeDB;

int main(){
    FudgeDB *fudgeDB = FudgeDB::getFudgDB("");
    fudgeDB->open();
    fudgeDB->run();
    fudgeDB->close();
}
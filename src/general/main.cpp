#include "FudgeDB.h"
#include "TableCatalog.h"
#include "Type.h"
#include "TupleDesc.h"
#include "Table.h"
#include <iostream>

#include <string>

using namespace fudgeDB;

int main(){
    FudgeDB *fudgeDB = FudgeDB::getFudgDB("");
    fudgeDB->open();
    fudgeDB->run();
    fudgeDB->close();
}
#include "gtest/gtest.h"
#include "general/FudgeDB.h"
#include "general/TableCatalog.h"
#include "general/Type.h"
#include "general/TupleDesc.h"
#include "general/Table.h"
#include "general/fudgeDBError.h"

#include <string>

using namespace fudgeDB;

TEST(TestCreateTable, ReloadTableCatalog){
    try{
        FudgeDB *fudgeDB = FudgeDB::getFudgDB("");
        fudgeDB->open();
        TableCatalog* tableCatalog = fudgeDB->getTableCatalog();
        std::vector<std::string> names{"col1", "col2"};
        std::vector<Type*> types1{new IntType(), new StringType(100)};
        std::vector<Type*> types2{new IntType(), new StringType(100)};
        TupleDesc* tupleDesc1 = new TupleDesc(names, types1);
        TupleDesc* tupleDesc2 = new TupleDesc(names, types2);
        tableCatalog->insertTable("TestTable1", new Table("TestTable1", tupleDesc1, 321, 0));
        tableCatalog->insertTable("TestTable2", new Table("TestTable2", tupleDesc2, 1, 3211));
        tableCatalog->updateCatalog();
        fudgeDB->close();
        fudgeDB->open();
        tableCatalog = fudgeDB->getTableCatalog();
        ASSERT_TRUE(tableCatalog->getTable("TestTable1") != nullptr);
        ASSERT_TRUE(tableCatalog->getTable("TestTable2") != nullptr);
        ASSERT_TRUE(tableCatalog->getTable("TestTable1")->getTupleDesc()->getLength() == 2);
        ASSERT_TRUE(tableCatalog->getTable("TestTable2")->getTupleDesc()->
            getTypes()->at(0)->getType() == TypeEnum::IntTypeEnum);
        ASSERT_TRUE(tableCatalog->getTable("TestTable2")->getTupleDesc()->
            getTypes()->at(1)->getType() == TypeEnum::StringTypeEnum);
        fudgeDB->close();
    }catch(fudgeError e){
        std::cout<<e.what()<<std::endl;
    }
}
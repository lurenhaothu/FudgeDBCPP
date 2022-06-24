#include "gtest/gtest.h"
#include "general/FudgeDB.h"
#include "general/TableCatalog.h"
#include "general/Type.h"
#include "general/TupleDesc.h"
#include "general/Table.h"
#include "general/Tuple.h"
#include "general/Field.h"
#include "general/TupleIterator.h"
#include "general/MemBuffer.h"

#include <string>
#include <iostream>

using namespace fudgeDB;

TEST(InsertTest, InsertTupleTest){
    FudgeDB *fudgeDB = FudgeDB::getFudgDB("");
    fudgeDB->open();
    TableCatalog* tableCatalog = fudgeDB->getTableCatalog();
    std::vector<std::string> names{"msgID", "msgCont", "time"};
    std::vector<Type*> types1{new IntType(), new StringType(20), new IntType()};
    TupleDesc* tupleDesc1 = new TupleDesc(names, types1);
    tableCatalog->insertTable("TestTable3", new Table("TestTable3", tupleDesc1, 0, -1));
    tableCatalog->updateCatalog();
    Table* table = tableCatalog->getTable("TestTable3");

    std::vector<std::string> dic{"Hello world!", "How are you?", "This is a simple test!", "Hope you make it!"};
    int tupleNum = 10000;
    for(int i = 0; i < tupleNum; i++){
        //std::cout<<i<<std::endl;
        auto tuple = new Tuple(tupleDesc1, std::vector<Field*>{new IntField(i), new StringField(dic[i % 4]), new IntField(i % 19)});
        table->insertTuple(tuple);
        //std::cout<<"finish insert"<<std::endl;
    }
    fudgeDB->close();
}

TEST(ReopenInsertTest, InsertTupleTest){
    FudgeDB *fudgeDB = FudgeDB::getFudgDB("");
    fudgeDB->open();
    std::vector<std::string> names{"msgID", "msgCont", "time"};
    std::vector<Type*> types1{new IntType(), new StringType(20), new IntType()};
    TupleDesc* tupleDesc1 = new TupleDesc(names, types1);
    TableCatalog* tableCatalog = fudgeDB->getTableCatalog();
    Table* table = tableCatalog->getTable("TestTable3");

    std::vector<std::string> dic{"Hello world!", "How are you?", "This is a simple test!", "Hope you make it!"};
    int tupleNum = 10000;
    for(int i = 0; i < tupleNum; i++){
        //std::cout<<i<<std::endl;
        auto tuple = new Tuple(tupleDesc1, std::vector<Field*>{new IntField(i), new StringField(dic[i % 4]), new IntField(i % 19)});
        table->insertTuple(tuple);
        //std::cout<<"finish insert"<<std::endl;
    }
    fudgeDB->close();
}

TEST(HeapPageIterator, InsertTupleTest){
    FudgeDB *fudgeDB = FudgeDB::getFudgDB("");
    fudgeDB->open();

    TableCatalog* tableCatalog = fudgeDB->getTableCatalog();
    Table* table = tableCatalog->getTable("TestTable3");

    auto page = fudgeDB->getMemBuffer()->getPage(table, 0, false);
    auto itr = page->getIterator();
    itr->open();
    for(int i = 0; i < 128; i++){
        ASSERT_TRUE(itr->hasNext());
        auto nextTuple = itr->next();
        ASSERT_TRUE(nextTuple->getField(0)->getType() == FieldType::intField);
        //ASSERT_TRUE(dynamic_cast<IntField*>(nextTuple->getField(0))->getValue() == i);
    }
    ASSERT_FALSE(itr->hasNext());
    fudgeDB->getMemBuffer()->releasePage(table, 0);
    fudgeDB->close();
    
}

TEST(TableIterator, InsertTupleTest){
    FudgeDB *fudgeDB = FudgeDB::getFudgDB("");
    fudgeDB->open();

    TableCatalog* tableCatalog = fudgeDB->getTableCatalog();
    Table* table = tableCatalog->getTable("TestTable3");

    auto itr = table->getIterator();
    itr->open();
    for(int i = 0; i < 10000; i++){
        ASSERT_TRUE(itr->hasNext());
        auto nextTuple = itr->next();
        ASSERT_TRUE(nextTuple->getField(0)->getType() == FieldType::intField);
        ASSERT_TRUE(dynamic_cast<IntField*>(nextTuple->getField(0))->getValue() == i);
    }
    ASSERT_TRUE(itr->hasNext());
    fudgeDB->close();
}
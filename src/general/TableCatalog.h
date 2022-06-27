#ifndef FUDGEDB_SRC_GENERAL_TABLECATALOG_H
#define FUDGEDB_SRC_GENERAL_TABLECATALOG_H

#include <string>
#include <unordered_map>

namespace fudgeDB{
    class Table;

    class TableCatalog{
        private:
            std::string CatalogFile;
            std::unordered_map<std::string, Table*> tableMap;
        public:
            TableCatalog(std::string CatalogFile); //load from file
            ~TableCatalog();
            void insertTable(std::string name, Table* table);
            // Return nullptr if table name not exist
            Table* getTable(std::string name);
        public:
            void updateCatalog();
    };
}

//table catalog的需求：
//1. 可以增减表格->tablecatalog类更新
//2. 可以实时更新表格信息, 包括id, pageNum, avaiHead->需要又table类来更新
//3. tupleDesc信息是不定长的，因为包含每列的名字
//每个表都有从string读取的构造函数, 读tablecatalog的时候记录每个string的长度，

#endif
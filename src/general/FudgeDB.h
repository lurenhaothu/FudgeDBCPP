#ifndef FUDGEDB_SRC_GENERAL_FUDGEDB_H
#define FUDGEDB_SRC_GENERAL_FUDGEDB_H

#include <string>

namespace fudgeDB{
    class TableCatalog;
    class MemBuffer;
    class Executor;

    class FudgeDB{
        public:
            const std::string tableCatalogName = "Catalog.fudgeDBCat";
        private:
            std::string DBPath;
            TableCatalog* tableCatalog;
            MemBuffer* memBuffer;
            Executor* executor;  
            static FudgeDB* singletonFudgeDB;
            FudgeDB(std::string DBPath);
        public:
            static FudgeDB* getFudgDB(std::string DBPath);
            static FudgeDB* getFudgDB();
            ~FudgeDB();
            TableCatalog* getTableCatalog();
            MemBuffer* getMemBuffer();
            void open();
            void close();
            void run();
    };
}

#endif
#ifndef FUDGEDB_SRC_GENERAL_EXECUTOR_H
#define FUDGEDB_SRC_GENERAL_EXECUTOR_H

#include <string>

namespace hsql{
    class SQLStatement;
    class CreateStatement;
    class InsertStatement;
    class SelectStatement;
}

namespace fudgeDB{
    class Executor{
        public:
            Executor() = default;
            std::string executeStatement(const hsql::SQLStatement* statement);
        private:
            std::string executeCreateStatement(const hsql::CreateStatement* statement);
            std::string executeInsertStatement(const hsql::InsertStatement* statement);
            std::string executeSelectStatement(const hsql::SelectStatement* statement);
        private:
            std::string createTableHelper(const hsql::CreateStatement* statement);
            std::string insertValueHelper(const hsql::InsertStatement* statement);
    };
}

#endif
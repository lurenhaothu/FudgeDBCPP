#ifndef FUDGEDB_SRC_GENERAL_TUPLEDESC_H
#define FUDGEDB_SRC_GENERAL_TUPLEDESC_H

#include <string>
#include <vector>

namespace fudgeDB{
    class Type;
    class TupleDesc{
        private:
            std::vector<std::string> names;
            std::vector<Type*> types;
            int size;
            std::vector<std::string> tableNames;
            std::vector<std::string> alias;
        public:
            TupleDesc(std::vector<std::string> names, std::vector<Type*> types);
            TupleDesc(std::vector<std::string> names, std::vector<Type*> types, std::vector<std::string> tableNames, std::vector<std::string> alias); //TODO
            TupleDesc(TupleDesc* tupleDesc, std::vector<std::string> tableNames, std::vector<std::string> alias); //TODO
            ~TupleDesc();
            int getSize(); //byte size of the tuple
            int getLength(); //col number
            std::vector<std::string>* getNames();
            std::vector<Type*>* getTypes();
            std::string getName(int index);
            Type* getType(int index);

            std::string toString();

            int getIndex(std::string table, std::string colName); // table might also be alias name TODO
    };
}

#endif
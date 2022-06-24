#ifndef FUDGEDB_SRC_GENERAL_FUDGEDBERROR_H
#define FUDGEDB_SRC_GENERAL_FUDGEDBERROR_H

#include <exception>
#include <string>

namespace fudgeDB{
    class fudgeError : public std::exception{
        private:
            std::string errMsg;
        public:
            fudgeError(std::string errMsg){
                this->errMsg = errMsg;
            }
        const char* what(){
            return errMsg.c_str();
        }
    };
}

#endif
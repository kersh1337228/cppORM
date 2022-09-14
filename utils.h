#pragma once
#ifndef CARSERV_UTILS_H
#define CARSERV_UTILS_H
#include <sqlite3.h>


enum ErrorType {
    DATABASE = 0,
    AUTHENTICATION = 1,
    REQUEST = 2,
};

enum Role {
    CUSTOMER = 0,
    WORKER = 1,
    MANAGER = 2,
};

enum RequestState {
    CREATED = 0,
    ACCEPTED = 1,
    PROCESSING = 2,
    PROCESSED = 3,
    PAYED = 4,
    CLOSED = 5
};

enum WorkType {
    DIAGNOSTICS = 0,
    PREVENTION = 1,
    REPAIR = 2
};


struct Error : public std::exception {
    std::string message;
    ErrorType type;
    Error(std::string message, ErrorType type):
            message(std::move(message)), type(type) {}
};


std::string* unpack_row(sqlite3_stmt*);

std::vector<std::string*> unpack_rows(sqlite3_stmt*);


#endif

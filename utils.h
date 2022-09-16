#pragma once
#ifndef CARSERV_UTILS_H
#define CARSERV_UTILS_H
#include <sqlite3.h>

enum ErrorType {
    DATABASE = 0,
    MODEL = 1,
};

//enum Role {
//    CUSTOMER = 0,
//    WORKER = 1,
//    MANAGER = 2,
//};

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

const std::string states[6] = {
    "Created", "Accepted", "Processing", "Processed", "Payed", "Closed"
};

const std::string worktypes[3] = {
    "Diagnostics", "Prevention", "Repair"
};

struct Error : public std::exception {
    std::string message;
    ErrorType type;
    Error(std::string message, ErrorType type):
            message(std::move(message)), type(type) {}
};

template<typename T>
T recast(const std::string& s);
template<>
std::string recast<std::string>(const std::string& s);
template<>
int recast<int>(const std::string& s);
template<>
std::chrono::system_clock::time_point recast<std::chrono::system_clock::time_point>(const std::string& s);

std::vector<std::string> unpack_row(sqlite3_stmt*);
std::vector<std::vector<std::string>> unpack_rows(sqlite3_stmt*);

#endif

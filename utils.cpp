#pragma once
#include "sqlite3.h"
#include <string>
#include <chrono>
#include <array>

std::vector<std::string> unpack_row(sqlite3_stmt* stmt) {
    size_t columns = sqlite3_data_count(stmt);
    std::vector<std::string> row;
    for (unsigned int i = 0; i < columns; i++) {
        if (sqlite3_column_type(stmt, i) != SQLITE_NULL) {
            row[i] = reinterpret_cast<const char*>(
                sqlite3_column_text(stmt, i)
            );
        } else {
            row[i] = "NULL";
        }
    }
    return row;
}
std::vector<std::vector<std::string>> unpack_rows(sqlite3_stmt* stmt) {
    std::vector<std::vector<std::string>> rows;
    while (sqlite3_step(stmt) == SQLITE_ROW)
        rows.emplace_back(unpack_row(stmt));
    return rows;
}

enum ErrorType {
    DATABASE = 0,
    MODEL = 1,
};

struct Error : public std::exception {
    std::string message;
    ErrorType type;
    Error(std::string message, ErrorType type):
    message(std::move(message)), type(type) {};
};

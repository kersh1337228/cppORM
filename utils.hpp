#pragma once
#include "sqlite3.h"
#include <string>
#include <chrono>
#include <map>
#include <array>

template<typename T1, typename T2>
std::tuple<std::vector<T1>, std::vector<T2>> keysnvals(std::map<T1, T2> const& m) {
    std::vector<T1> keys;
    std::vector<T2> values;
    for (auto it = m.begin(); it !=  m.end(); ++it) {
        keys.emplace_back(it->first);
        values.emplace_back(it->second);
    }
    return std::make_tuple(keys, values);
}

std::map<std::string, std::string> unpack_row(sqlite3_stmt* stmt) {
    size_t columns = sqlite3_data_count(stmt);
    std::map<std::string, std::string> row;
    for (unsigned int i = 0; i < columns; i++) {
        if (sqlite3_column_type(stmt, i) != SQLITE_NULL) {
            row[sqlite3_column_name(stmt, i)] = reinterpret_cast<const char*>(
                sqlite3_column_text(stmt, i)
            );
        } else {
            row[sqlite3_column_name(stmt, i)] = "NULL";
        }
    }
    return row;
}
std::vector<std::map<std::string, std::string>> unpack_rows(sqlite3_stmt* stmt) {
    std::vector<std::map<std::string, std::string>> rows;
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

#include <string>
#include <vector>
#include <array>
#include <sqlite3.h>
#include <chrono>
#include "utils.h"
#include "orm/Field.h"

template<typename T>
T recast(const std::string& s) {
    return s != "NULL" ? static_cast<T>(std::stoi(s)) : static_cast<T>(INT_MIN);
}
template<>
std::string recast<std::string>(const std::string& s) {
    return CharField::from_sql(s);
}
template<>
int recast<int>(const std::string& s) {
    return IntField::from_sql(s);
}
template<>
std::chrono::system_clock::time_point recast<std::chrono::system_clock::time_point>(const std::string& s) {
    return DateTimeField::from_sql(s);
}

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


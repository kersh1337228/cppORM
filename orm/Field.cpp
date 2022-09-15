#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
#include "Field.h"

// ____________________Field____________________
template <typename T>
std::string Field<T>::tname() const {
    return typeid(T).name();
}

// ____________________CharField____________________
constexpr std::string CharField::init() const {
    std::string sql = "VARCHAR(" + std::to_string(this->size) + ")";
    if (!this->null) sql += " NOT NULL";
    if (this->unique) sql += " UNIQUE";
    return sql;
}
constexpr std::string CharField::to_sql() const {
    return this->value.empty() ? "NULL" : "'" + this->value + "'";
}
constexpr std::string CharField::from_sql(const std::string& sql) const {
    return sql != "NULL" ? sql : "";
}

// ____________________IntField____________________
constexpr std::string IntField::init() const {
    std::string sql = "INTEGER";
    if (!this->null) sql += " NOT NULL";
    if (this->unique) sql += " UNIQUE";
    return this->foreign_key ? sql :
           sql + ", FOREIGN KEY (" + this->name + ") REFERENCES "+ this->reftb + "(id)";
}
constexpr std::string IntField::to_sql() const {
    return this->value == INT_MIN ? "NULL" : std::to_string(this->value);
}
constexpr int IntField::from_sql(const std::string& sql) const {
    return sql != "NULL" ? std::stoi(sql) : INT_MIN;
}

// ____________________DateTimeField____________________
std::string DateTimeField::tps(const std::chrono::system_clock::time_point& tp) {
    time_t t = std::chrono::system_clock::to_time_t(tp);
    std::stringstream ss;
    ss << std::put_time(localtime(&t), "%d/%m/%Y %X");
    return ss.str();
}
std::chrono::system_clock::time_point DateTimeField::stp(const std::string& s) {
    tm tm = {};
    std::stringstream ss(s);
    ss >> std::get_time(&tm, "%d/%m/%Y %X");
    return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}
constexpr std::string DateTimeField::init() const {
    std::string sql = "TEXT";
    if (!this->null) sql += " NOT NULL";
    if (this->unique) sql += " UNIQUE";
    return sql;
}
constexpr std::string DateTimeField::to_sql() const {
    return this->value == std::chrono::system_clock::time_point::min() ? "NULL" :
    "'" + DateTimeField::tps(this->value) + "'";
}
constexpr std::chrono::system_clock::time_point DateTimeField::from_sql(const std::string& sql) const {
    return sql != "NULL" ? DateTimeField::stp(sql) :
    std::chrono::system_clock::time_point::min();
}

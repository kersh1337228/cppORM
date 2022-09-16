#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>
#include <array>
#include "Field.h"

// ____________________BasicField____________________
constexpr std::string BasicField::init() const {
    std::string sql;
    if (!this->null) sql += " NOT NULL";
    if (this->unique) sql += " UNIQUE";
    return sql;
}

// ____________________CharField____________________
constexpr std::string CharField::init() const {
    std::string sql = "VARCHAR(" + std::to_string(this->size) + ")";
    return sql + this->BasicField::init();
}
constexpr std::string CharField::to_sql() const {
    return this->value.empty() ? "NULL" : "'" + this->value + "'";
}
constexpr std::string CharField::from_sql(const std::string& sql) {
    return sql != "NULL" ? sql : "";
}
constexpr std::string CharField::print() const {
    return this->value.empty() ? "<null>" : this->value;
}

// ____________________IntField____________________
constexpr std::string IntField::init() const {
    std::string sql = "INTEGER" + this->BasicField::init();
    return this->foreign_key ? sql :
           sql + ", FOREIGN KEY (" + this->name + ") REFERENCES "+ this->reftb + "(id)";
}
constexpr std::string IntField::to_sql() const {
    return this->value == INT_MIN ? "NULL" : std::to_string(this->value);
}
constexpr int IntField::from_sql(const std::string& sql) {
    return sql != "NULL" ? std::stoi(sql) : INT_MIN;
}
constexpr std::string IntField::print() const {
    return this->value == INT_MIN ? "<null>" : std::to_string(this->value);
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
    std::string sql = "TEXT" + this->BasicField::init();
    return this->null ? sql : sql + "DEFAULT '" + tps(this->value) + "'";
}
constexpr std::string DateTimeField::to_sql() const {
    return this->value == std::chrono::system_clock::time_point::min() ? "NULL" :
    "'" + DateTimeField::tps(this->value) + "'";
}
constexpr std::chrono::system_clock::time_point DateTimeField::from_sql(const std::string& sql) {
    return sql != "NULL" ? DateTimeField::stp(sql) :
    std::chrono::system_clock::time_point::min();
}
constexpr std::string DateTimeField::print() const {
    return this->value == std::chrono::system_clock::time_point::min() ? "<null>" :
    DateTimeField::tps(this->value);
}

// ____________________EnumField____________________
template <typename T, size_t N>
constexpr std::string EnumField<T, N>::init() const {
    std::string sql = "INTEGER";
    return sql + this->BasicField::init();
}
template <typename T, size_t N>
constexpr std::string EnumField<T, N>::to_sql() const {
    return this->value == static_cast<T>(INT_MIN) ? "NULL" : std::to_string(this->value);
}
template <typename T, size_t N>
constexpr T EnumField<T, N>::from_sql(const std::string& sql) {
    return sql != "NULL" ? static_cast<T>(std::stoi(sql)) : static_cast<T>(INT_MIN);
}
template <typename T, size_t N>
constexpr std::string EnumField<T, N>::print() const {
    return this->value == static_cast<T>(INT_MIN) ? "<null>" : this->names[this->value];
}

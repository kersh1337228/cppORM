#pragma once
#include <string>
#include <chrono>

class BasicField {
protected:
    bool null;
    bool unique;
public:
    BasicField(): null(true), unique(false) {};
    BasicField(bool null, bool unique): null(null), unique(unique) {};
    virtual constexpr std::string init() const {
        std::string sql;
        if (!this->null) sql += " NOT NULL";
        if (this->unique) sql += " UNIQUE";
        return sql;
    }
    virtual constexpr std::string to_sql() const = 0;
    virtual constexpr std::string print() const  = 0;
};

template <typename T>
class Field : public BasicField {
public:
    T value;
    // Placeholder (no-value) constructors
    Field() = default;
    Field(bool null, bool unique): BasicField(null, unique) {};
    // Value-init constructors
    Field(T value): value(value) {}
    Field(T value, bool null, bool unique): value(value), BasicField(null, unique) {};
    // Query-aimed methods
    virtual constexpr std::string init() const override = 0;
    virtual constexpr std::string to_sql() const override = 0;
    static constexpr T from_sql(const std::string& sql);
    virtual constexpr std::string print() const override = 0;
};

template <>
class Field<std::string> : public BasicField {
public:
    std::string value;
    size_t size;
    // Placeholder (no-value) constructors
    Field(): value(""), size(255) {};
    Field(bool null, bool unique): value(""), size(255), BasicField(null, unique) {};
    // Value-init constructors
    Field(const std::string& value): value(value), size(value.size()) {};
    Field(const std::string& value, bool null, bool unique):
    BasicField(null, unique), value(value), size(value.size()) {};
    // Query-aimed methods
    constexpr std::string init() const override {
        std::string sql = " VARCHAR(" + std::to_string(this->size) + ")";
        return sql + this->BasicField::init();
    }
    constexpr std::string to_sql() const override {
        return this->value.empty() ? "NULL" : "'" + this->value + "'";
    }
    static constexpr std::string from_sql(const std::string& sql) {
        return sql != "NULL" ? sql : "";
    }
    constexpr std::string print() const override {
        return this->value.empty() ? "<null>" : this->value;
    }
};

template <>
class Field<int> : public BasicField {
private:
    bool foreign_key;
    std::string reftb;
    std::string name;
public:
    int value;
    // Non-foreign_key constructors
    // Placeholder (no-value) constructors
    Field(): value(-1), foreign_key(false) {};
    Field(bool null, bool unique): value(-1), foreign_key(false), BasicField(null, unique) {};
    // Value-init constructors
    Field(int value): value(value), foreign_key(false) {};
    Field(int value, bool null, bool unique): value(value), foreign_key(false), BasicField(null, unique) {};
    // Foreign_key constructors
    // Placeholder (no-value) constructors
    Field(const std::string& name, const std::string& reftb):
    value(-1), name(name), foreign_key(true), reftb(reftb) {};
    Field(const std::string& name, bool null, bool unique, const std::string& reftb):
    value(-1), BasicField(null, unique), name(name), foreign_key(true), reftb(reftb) {};
    // Value-init constructors
    Field(const std::string& name, int value, const std::string& reftb):
    value(value), name(name), foreign_key(true), reftb(reftb) {};
    Field(const std::string& name, int value, bool null, bool unique, const std::string& reftb):
    value(value), BasicField(null, unique), name(name), foreign_key(true), reftb(reftb) {};
    constexpr std::string init() const override {
        std::string sql = " INTEGER" + this->BasicField::init();
        return !this->foreign_key ? sql :
               sql + ", FOREIGN KEY (" + this->name + ") REFERENCES "+ this->reftb + "(id)";
    }
    constexpr std::string to_sql() const override {
        return this->value == -1 ? "NULL" : std::to_string(this->value);
    }
    static constexpr int from_sql(const std::string& sql) {
        return sql != "NULL" ? std::stoi(sql) : -1;
    }
    constexpr std::string print() const override {
        return this->value == -1 ? "<null>" : std::to_string(this->value);
    }
};

template <>
class Field<std::chrono::system_clock::time_point> : public BasicField {
public:
    std::chrono::system_clock::time_point value;
    // Placeholder (no-value) constructors
    Field(): value(std::chrono::system_clock::time_point::min()) {};
    Field(bool null, bool unique):
    value(std::chrono::system_clock::time_point::min()), BasicField(null, unique) {};
    // Value-init constructors
    Field(const std::chrono::system_clock::time_point& value): value(value) {}
    Field(
        const std::chrono::system_clock::time_point& value,
        bool null, bool unique
    ): value(value), BasicField(null, unique) {};
    static std::string tps(const std::chrono::system_clock::time_point& tp) {
        time_t t = std::chrono::system_clock::to_time_t(tp);
        std::stringstream ss;
        ss << std::put_time(localtime(&t), "%d/%m/%Y %X");
        return ss.str();
    }
    static std::chrono::system_clock::time_point stp(const std::string& s) {
        tm tm = {};
        std::stringstream ss(s);
        ss >> std::get_time(&tm, "%d/%m/%Y %X");
        return std::chrono::system_clock::from_time_t(std::mktime(&tm));
    }
    constexpr std::string init() const override {
        std::string sql = " TEXT" + this->BasicField::init();
        return this->null ? sql : sql + " DEFAULT '" + tps(this->value) + "'";
    }
    constexpr std::string to_sql() const override {
        return this->value == std::chrono::system_clock::time_point::min() ? "NULL" :
               "'" + Field::tps(this->value) + "'";
    }
    static constexpr std::chrono::system_clock::time_point from_sql(const std::string& sql) {
        return sql != "NULL" ? Field::stp(sql) :
               std::chrono::system_clock::time_point::min();
    }
    constexpr std::string print() const override {
        return this->value == std::chrono::system_clock::time_point::min() ? "<null>" :
        Field::tps(this->value);
    }
};

template<typename T>
T recast(const std::string& s) {
    return s != "NULL" ? static_cast<T>(s) : static_cast<T>(-1);
}
template<>
std::string recast<std::string>(const std::string& s) {
    return Field<std::string>::from_sql(s);
}
template<>
int recast<int>(const std::string& s) {
    return Field<int>::from_sql(s);
}
template<>
std::chrono::system_clock::time_point recast<std::chrono::system_clock::time_point>(const std::string& s) {
    return Field<std::chrono::system_clock::time_point>::from_sql(s);
}

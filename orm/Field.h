#pragma once
#ifndef CARSERV_FIELD_H
#define CARSERV_FIELD_H

class BasicField {
protected:
    bool null = true;
    bool unique = false;
public:
    std::string name;
    BasicField(const std::string& name): name(name) {};
    BasicField(const std::string& name, bool null, bool unique):
    name(name), null(null), unique(unique) {};
};

template <typename T>
class Field : public BasicField {
public:
    T value;
    std::string name;
    Field(T value): value(value) {}
    Field(const std::string& name, T value): name(name), value(value) {}
    Field(T value, bool null, bool unique): value(value), null(null), unique(unique) {};
    Field(const std::string& name, T value, bool null, bool unique):
    name(name), value(value), null(null), unique(unique) {};
    virtual constexpr std::string init() const = 0;
    virtual constexpr std::string to_sql() const = 0;
    virtual constexpr T from_sql(const std::string& sql) const = 0;
};

class CharField : public Field<std::string> {
public:
    size_t size = 255;
    CharField(const std::string& name, const std::string& value):
    Field(name, value), size(value.size()) {};
    CharField(const std::string& name, const std::string& value, bool null, bool unique):
    Field(name, value, null, unique), size(value.size()) {};
    constexpr std::string init() const override;
    constexpr std::string to_sql() const override;
    constexpr std::string from_sql(const std::string& sql) const override;
};

class IntField : public Field<int> {
private:
    bool foreign_key = false;
    std::string reftb;
public:
    // Non-foreign_key constructors
    IntField(const std::string& name, int value):
    Field(name, value) {};
    IntField(const std::string& name, int value, const std::string& reftb):
    Field(name, value), foreign_key(true), reftb(reftb) {};
    // Foreign_key constructors
    IntField(const std::string& name, int value, bool null, bool unique):
    Field(name, value, null, unique) {};
    IntField(const std::string& name, int value, bool null, bool unique, const std::string& reftb):
    Field(name, value, null, unique), foreign_key(true), reftb(reftb) {};
    constexpr std::string init() const override;
    constexpr std::string to_sql() const override;
    constexpr int from_sql(const std::string& sql) const override;
};

class DateTimeField : public Field<std::chrono::system_clock::time_point> {
public:
    DateTimeField(const std::string& name): Field(name, std::chrono::system_clock::now()) {}
    DateTimeField(const std::string& name, const std::chrono::system_clock::time_point& value):
    Field(name, value) {}
    DateTimeField(
        const std::string& name,
        const std::chrono::system_clock::time_point& value,
        bool null, bool unique
    ): Field(name, value, null, unique) {};
    static std::string tps(const std::chrono::system_clock::time_point& tp);
    static std::chrono::system_clock::time_point stp(const std::string& s);
    constexpr std::string init() const override;
    constexpr std::string to_sql() const override;
    constexpr std::chrono::system_clock::time_point from_sql(const std::string& sql) const override;
};

#endif

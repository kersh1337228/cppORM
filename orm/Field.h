#pragma once
#ifndef CARSERV_FIELD_H
#define CARSERV_FIELD_H

class BasicField {
protected:
    bool null = true;
    bool unique = false;
public:
    BasicField() = default;
    BasicField(bool null, bool unique): null(null), unique(unique) {};
    virtual constexpr std::string init() const = 0;
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
    virtual constexpr std::string print() const override = 0;
};

class CharField : public Field<std::string> {
public:
    size_t size = 255;
    // Placeholder (no-value) constructors
    CharField(): Field("") {};
    CharField(bool null, bool unique): Field("", null, unique) {};
    // Value-init constructors
    CharField(const std::string& value): Field(value), size(value.size()) {};
    CharField(const std::string& value, bool null, bool unique):
    Field(value, null, unique), size(value.size()) {};
    // Query-aimed methods
    constexpr std::string init() const override;
    constexpr std::string to_sql() const override;
    static constexpr std::string from_sql(const std::string& sql);
    constexpr std::string print() const override;
};

class IntField : public Field<int> {
private:
    bool foreign_key = false;
    std::string reftb;
    std::string name;
public:
    // Non-foreign_key constructors
    // Placeholder (no-value) constructors
    IntField(): Field(INT_MIN) {};
    IntField(bool null, bool unique): Field(INT_MIN, null, unique) {};
    // Value-init constructors
    IntField(int value): Field(value) {};
    IntField(int value, bool null, bool unique): Field(value, null, unique) {};
    // Foreign_key constructors
    // Placeholder (no-value) constructors
    IntField(const std::string& name, const std::string& reftb):
    Field(INT_MIN), name(name), foreign_key(true), reftb(reftb) {};
    IntField(const std::string& name, bool null, bool unique, const std::string& reftb):
    Field(INT_MIN, null, unique), name(name), foreign_key(true), reftb(reftb) {};
    // Value-init constructors
    IntField(const std::string& name, int value, const std::string& reftb):
    Field(value), name(name), foreign_key(true), reftb(reftb) {};
    IntField(const std::string& name, int value, bool null, bool unique, const std::string& reftb):
    Field(value, null, unique), name(name), foreign_key(true), reftb(reftb) {};
    constexpr std::string init() const override;
    constexpr std::string to_sql() const override;
    static constexpr int from_sql(const std::string& sql);
    constexpr std::string print() const override;
};

class DateTimeField : public Field<std::chrono::system_clock::time_point> {
public:
    // Placeholder (no-value) constructors
    DateTimeField(): Field(std::chrono::system_clock::time_point::min()) {};
    DateTimeField(bool null, bool unique):
    Field(std::chrono::system_clock::time_point::min(), null, unique) {};
    // Value-init constructors
    DateTimeField(const std::chrono::system_clock::time_point& value): Field(value) {}
    DateTimeField(
        const std::chrono::system_clock::time_point& value,
        bool null, bool unique
    ): Field(value, null, unique) {};
    static std::string tps(const std::chrono::system_clock::time_point& tp);
    static std::chrono::system_clock::time_point stp(const std::string& s);
    constexpr std::string init() const override;
    constexpr std::string to_sql() const override;
    static constexpr std::chrono::system_clock::time_point from_sql(const std::string& sql);
    constexpr std::string print() const override;
};

template <typename T, size_t N>
class EnumField : public Field<T> {
    std::array<std::string, N> names = {};
public:
    // Placeholder (no-value) constructors
    EnumField(const std::array<std::string, N>& names):
    Field<T>(static_cast<T>(INT_MIN)), names(std::move(names)) {};
    EnumField(const std::array<std::string, N>& names, bool null, bool unique):
    Field<T>(static_cast<T>(INT_MIN), null, unique), names(std::move(names)) {};
    // Value-init constructors
    EnumField(T value): Field<T>(value) {}
    EnumField(T value,bool null, bool unique): Field<T>(value, null, unique) {};
    EnumField(const std::array<std::string, N>& names, T value): Field<T>(value), names(std::move(names)) {}
    EnumField(const std::array<std::string, N>& names, T value,bool null, bool unique):
    Field<T>(value, null, unique), names(std::move(names)) {};
    constexpr std::string init() const override;
    constexpr std::string to_sql() const override;
    static constexpr T from_sql(const std::string& sql);
    constexpr std::string print() const override;
};

#endif

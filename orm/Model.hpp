#pragma once
#include "../utils.hpp"
#include "Field.hpp"
#include "sqlite3.h"
#include <string>
#include <map>
#include <vector>
//#include <ostream>

//template <typename... Types>
//class Model;
//
//template<typename... Types, size_t... Idxs>
//Model<Types...>* parse(std::vector<std::string> const& keys, std::vector<std::string> const& values, std::index_sequence<Idxs...>);
//template <typename... Types>
//Model<Types...>* unpack(unsigned int id, std::vector<std::string> const& keys, std::vector<std::string> const& values);

template <typename... Types>
class Model {
public:
    unsigned int id;
    std::map<std::string, BasicField*> fields;
    std::string tbname;
    Model(  // Table-init constructor
        const std::string& tbname,
        const std::map<std::string, BasicField*>& fields
    ): fields(fields), tbname(tbname) {
        sqlite3* db;
        std::string query = "CREATE TABLE IF NOT EXISTS " + this->tbname + " ("
         "id INTEGER PRIMARY KEY AUTOINCREMENT, ";
//        for (auto& field : fields) {
//            this->fields.insert(field);
//            query += field.first + field.second->init() + ", ";
//        }
        sqlite3_open("db.sqlite3", &db);
        char* error_message;
        if (sqlite3_exec(db, (query.substr(0, query.size() - 2) + ");").c_str(), nullptr, nullptr, &error_message) != SQLITE_OK) {
            sqlite3_free(error_message);
            throw Error(
                "Unknown error did happen during CREATE operation in " + this->tbname + " table", DATABASE
            );
        }
        sqlite3_close(db);
    }
    Model(const std::map<std::string, BasicField*>& fields): fields(fields) {};
    Model(Types... args) {
        std::vector<BasicField*> vals = {new Field<Types>(args)...};
        unsigned int i = 0;
        for (auto& pair : this->fields) {
            this->fields[pair.first] = vals[i];
            ++i;
        }
    }
    constexpr std::string header() const {
        std::string head;
        for (auto& pair : this->fields) head += pair.first + "\t";
        return head;
    }
    constexpr std::string print() const {
        std::string out;
        for (auto& field : this->fields) out += field.second->print() + "\t";
        return out;
    }
    // CRUD methods
    void create() const {
        sqlite3* db;
        sqlite3_open("db.sqlite3", &db);
        std::string query = "INSERT INTO " + this->tbname + " (";
        for (auto& pair : this->fields)
            query += pair.first + ", ";
        query = query.substr(0, query.size() - 2) + ") VALUES (";
        for (auto& pair : this->fields)
            query += pair.second->to_sql() + ", ";
        if (sqlite3_exec(db, (query.substr(0, query.size() - 2) + ");").c_str(), nullptr, nullptr, nullptr) != SQLITE_OK) {
            throw Error(
                "Unknown error did happen during INSERT operation in " +
                this->tbname + " table", DATABASE
            );
        }
        sqlite3_close(db);
    }
    static std::vector<Model*> read(const std::string& tbname, const std::map<std::string, std::string>& data) {
        sqlite3* db;
        sqlite3_open("db.sqlite3", &db);
        std::string query = "SELECT * FROM " + tbname + " WHERE id > 0";
        for(auto pair : data) query += " AND " + pair.first + " = " + pair.second;
        sqlite3_exec(  // Check if request exists
            db, std::string("SELECT EXISTS(" + query + ");").c_str(),
            [](void*, int, char** argv, char**) -> int {
                if (std::stoi(argv[0]) == 0)
                    throw Error("Not found", MODEL);
                return 0;
            }, nullptr, nullptr
        );
        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db, std::string(query + ";").c_str(), 999, &stmt, nullptr);
        std::vector<std::map<std::string, std::string>> rows = unpack_rows(stmt);
        std::vector<Model*> requests;
        for (auto row : rows) {
            auto knv = keysnvals<std::string, std::string>(row);
            std::vector<std::string> keys = std::get<0>(knv);
            std::vector<std::string> values = std::get<1>(knv);
            requests.emplace_back(unpack<Types...>(
                std::stoi(row["id"]), std::vector<std::string>(keys.begin() + 1, keys.end()),
                std::vector<std::string>(values.begin() + 1, values.end())
            ));
        }
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return requests;
    }
    void update(const std::map<std::string, std::string>& data)  {
        sqlite3* db;
        sqlite3_open("db.sqlite3", &db);
        std::string query = "UPDATE " + this->tbname + " SET ";
        for (auto& pair : data)
            query += pair.first + " = " + pair.second + ", ";
        query = query.substr(0, query.size() - 2) + " WHERE id = " + std::to_string(this->id);
        if (sqlite3_exec(db, query.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK) {
            throw Error(
                "Unknown error did happen during UPDATE operation in " +
                this->tbname + " table", DATABASE
            );
        }
        sqlite3_close(db);
    }
    void remove() const {
        sqlite3* db;
        sqlite3_open("db.sqlite3", &db);
        std::string query = "DELETE FROM " + this->tbname + " WHERE id = " + this->id;
        if (sqlite3_exec(db, query.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK) {
            throw Error(
                "Unknown error did happen during DELETE operation in " +
                this->tbname + " table", DATABASE
            );
        }
        sqlite3_close(db);
    }
};
template<typename... Types, size_t... Idxs>
Model<Types...>* parse(std::vector<std::string> const& keys, std::vector<std::string> const& values, std::index_sequence<Idxs...>){
    return new Model<Types...>({
        {keys[Idxs], new Field<Types>(recast<Types>(values[Idxs]))}...
    });
//    return new Model(recast<Types>(values[Idxs])...);
}
template <typename... Types>
Model<Types...>* unpack(unsigned int id, std::vector<std::string> const& keys, std::vector<std::string> const& values) {
    Model<Types...>* model = parse<Types...>(keys, values, std::make_index_sequence<sizeof...(Types)>{});
    model->id = id;
    return model;
}

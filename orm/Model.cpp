#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <memory>
#include "sqlite3.h"
#include "Field.h"
#include "Model.h"
#include "../utils.h"

Model::Model(const std::map<std::string, std::shared_ptr<BasicField>>& fields) {
    for (auto& field : fields) {
        this->fields.insert(field);
    }
}
Model::Model(const std::map<std::string, std::shared_ptr<Field>>& fields, const std::string& tbname) {
    sqlite3* db;
    std::string query = "CREATE TABLE IF NOT EXISTS " + tbname + " ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT, ";
    for (auto& field : fields) {
        this->fields.insert(field);
        query += field.first + field.second->init() + ", ";
    }
    sqlite3_open("db.sqlite3", &db);
    char* error_message;
    if (sqlite3_exec(db, (query.substr(0, query.size() - 2) + ");").c_str(), nullptr, nullptr, &error_message) != SQLITE_OK) {
        sqlite3_free(error_message);
        throw Error("Error during " + tbname + " table creation", DATABASE);
    }
    sqlite3_close(db);
}
void Model::create() {
    sqlite3* db;
    sqlite3_open("db.sqlite3", &db);
    std::string query = "INSERT INTO " + this->tbname + " (";
    for (auto& pair : this->fields)
        query += pair.first + ", ";
    query = query.substr(0, query.size() - 2) + ") VALUES (";
    for (auto& pair : this->fields)
        query += pair.second->to_sql() + ", ";
    if (sqlite3_exec(db, (query.substr(0, query.size() - 2) + ");").c_str(), nullptr, nullptr, nullptr) != SQLITE_OK) {
        throw Error("Unknown error did happen during" + this->tbname + "model creation", DATABASE);
    }
    sqlite3_close(db);
}
std::vector<Model*> Model::read(const std::map<std::string, std::string>& data) {
    sqlite3* db;
    sqlite3_open("db.sqlite3", &db);
    std::string query = "SELECT * FROM Requests WHERE id > 0";
    for(auto pair : data)
        query += " AND " + pair.first + " = " + pair.second;
    sqlite3_exec(  // Check if request exists
        db, std::string("SELECT EXISTS(" + query + ");").c_str(),
        [](void*, int, char** argv, char**) -> int {
            if (std::stoi(argv[0]) == 0)
                throw Error("No request found", REQUEST);
            return 0;
        },
        nullptr, nullptr
    );
    // Selecting requests
    sqlite3_stmt* stmt;
    // Unpacking rows
    sqlite3_prepare_v2(db, std::string(query + ";").c_str(), 999, &stmt, nullptr);
    std::vector<std::vector<std::string>> rows = unpack_rows(stmt);
    std::vector<Model*> requests;
    for (auto row : rows) {
        Model* model = new Model();
        model->id = stoi(row[0]);
        for (auto& field: this->fields) {
            model->fields[field.first] = field.second->from_sql();
        }
        requests.emplace_back(model);
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return requests;
}

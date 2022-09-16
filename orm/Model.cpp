#include "Field.h"
#include <string>
#include <map>
#include <vector>
#include <memory>
#include "sqlite3.h"
#include "Model.h"
#include "../utils.h"

template <typename... Types>
Model<Types...>::Model(Types... args) {
    std::vector<std::shared_ptr<BasicField>> vals = {new Field<Types>(args)...};
    unsigned int i = 0;
    for (auto& pair : this->fields) {
        this->fields[pair.first] = vals[i];
        ++i;
    }
}
template<typename... Types, size_t... Idxs>
std::unique_ptr<Model<Types...>> parse(std::vector<std::string> const& row, std::index_sequence<Idxs...>) {
    return std::unique_ptr(new Model(recast<Types>(row[Idxs])...));
}
template <typename... Types>
std::unique_ptr<Model<Types...>> unpack(unsigned int id, std::vector<std::string> const& row) {
    std::unique_ptr<Model<Types...>> model = parse<Types...>(row, std::make_index_sequence<sizeof...(Types)>{});
    model->id = id;
    return model;
}
template <typename... Types>
Model<Types...>::Model(const std::map<std::string, std::shared_ptr<BasicField>>& fields, const std::string& tbname) {
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
        throw Error(
            "Unknown error did happen during CREATE operation in " +
            this->tbname + " table", DATABASE
        );
    }
    sqlite3_close(db);
}
template <typename... Types>
void Model<Types...>::create() const {
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
template <typename... Types>
std::vector<std::unique_ptr<Model<Types...>>> Model<Types...>::read(const std::map<std::string, std::string>& data) const {
    sqlite3* db;
    sqlite3_open("db.sqlite3", &db);
    std::string query = "SELECT * FROM Requests WHERE id > 0";
    for(auto pair : data)
        query += " AND " + pair.first + " = " + pair.second;
    sqlite3_exec(  // Check if request exists
        db, std::string("SELECT EXISTS(" + query + ");").c_str(),
        [](void*, int, char** argv, char**) -> int {
            if (std::stoi(argv[0]) == 0)
                throw Error("Not found", MODEL);
            return 0;
        },
        nullptr, nullptr
    );
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, std::string(query + ";").c_str(), 999, &stmt, nullptr);
    std::vector<std::vector<std::string>> rows = unpack_rows(stmt);
    std::vector<std::unique_ptr<Model>> requests;
    for (auto row : rows)
        requests.emplace_back(unpack<Types...>(
            row[0], std::vector<std::string>(row.begin() + 1, row.end())
        ));
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return requests;
}
template <typename... Types>
void Model<Types...>::update(const std::map<std::string, std::string>& data) {
    sqlite3* db;
    sqlite3_open("db.sqlite3", &db);
    std::string query = "UPDATE Requests SET ";
    for (auto& pair : data)
        query += pair.first + " = " + pair.second + ", ";
    query = query.substr(0, query.size() - 2) + " WHERE id = " + this->id;
    if (sqlite3_exec(db, query.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK) {
        throw Error(
            "Unknown error did happen during UPDATE operation in " +
            this->tbname + " table", DATABASE
        );
    }
    sqlite3_close(db);
}
template <typename... Types>
void Model<Types...>::remove() const {
    sqlite3* db;
    sqlite3_open("db.sqlite3", &db);
    std::string query = "DELETE FROM Users WHERE id = " + this->id;
    if (sqlite3_exec(db, query.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK) {
        throw Error(
            "Unknown error did happen during DELETE operation in " +
            this->tbname + " table", DATABASE
        );
    }
    sqlite3_close(db);
}
template <typename... Types>
std::ostream& operator << (std::ostream& out, Model<Types...> model) {
    for (auto& field : model.fields)
        out << field.second->print() << "\t";
    return out;
}

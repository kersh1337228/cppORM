#include <iostream>
#include <string>
#include <chrono>
#include <map>
#include "../../orm/Field.h"
#include "../../orm/Model.h"
#include "Request.h"
#include "../../utils.h"

void Request::check_table() {
    sqlite3* db;
    string query = "CREATE TABLE IF NOT EXISTS Requests ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                   "state INTEGER DEFAULT 0, "
                   "worktype INTEGER NOT NULL, "
                   "customer INTEGER NOT NULL, "
                   "creation_date TEXT DEFAULT (datetime('now','localtime')) NOT NULL, "
                   "worker INTEGER, "
                   "closing_date TEXT, "
                   "close_reason TEXT, "
                   "FOREIGN KEY (worker) REFERENCES requests(id),"
                   "FOREIGN KEY (customer) REFERENCES requests(id));";
    sqlite3_open("db.sqlite3", &db);
    if (sqlite3_exec(db, query.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK) {
        throw Error("Error during Requests table creation", DATABASE);
    }
    sqlite3_close(db);
}

void Request::create(WorkType worktype, unsigned int userid) {
    Request::check_table();
    sqlite3* db;
    sqlite3_open("db.sqlite3", &db);
    string query = "INSERT INTO Requests (worktype, customer) VALUES (" +
                   to_string(worktype) + ", " + to_string(userid) + ");";
    if (sqlite3_exec(db, query.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK) {
        throw Error("Unknown error did happen during request creation", DATABASE);
    }
    sqlite3_close(db);
}

vector<Request> Request::read(
    map<const string, const string&> params
) {
    Request::check_table();
    sqlite3* db;
    sqlite3_open("db.sqlite3", &db);
    string query = "SELECT * FROM Requests WHERE id > 0";
    for(auto pair : params)
        if (!pair.second.empty()) query += " AND " + pair.first + " = " + pair.second;
    // Check if request exists
    sqlite3_exec(
            db, string("SELECT EXISTS(" + query + ");").c_str(),
            [](void*, int, char** argv, char**) -> int {
                if (stoi(argv[0]) == 0)
                    throw Error("No request found", REQUEST);
                return 0;
            },
            nullptr, nullptr
    );
    // Selecting requests
    sqlite3_stmt* stmt;
    // Unpacking rows
    sqlite3_prepare_v2(db, string(query + ";").c_str(), 999, &stmt, nullptr);
    vector<string*> rows = unpack_rows(stmt);
    vector<Request> requests;
    for (auto row : rows) {
        requests.emplace_back(Request(
            stoi(row[0]), static_cast<RequestState>(stoi(row[1])),
            static_cast<WorkType>(stoi(row[2])), row[3],
            row[4], row[5], row[6], row[7]
        ));
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return requests;
}

void Request::update(
        const string& new_worker="",
        const string& new_state="",
        const string& new_worktype="",
        const string& closing_date="",
        const string& close_reason=""
) {
    Request::check_table();
    Request::read({
        {"id", new_worker},
        {"customer", new_worker},
        {"worker", new_worker},
        {"id", id},
        {"id", id},
        {"id", id}
    });
    sqlite3* db;
    sqlite3_open("db.sqlite3", &db);
    string query = "UPDATE Requests SET ";
    if (!new_worker.empty()) query += " worker = " + new_worker + ", ";
    if (!new_state.empty()) query += " state = " + new_state + ", ";
    if (!new_worktype.empty()) query += " worktype = " + new_worktype + ", ";
    if (!closing_date.empty()) query += " closing_date = " + closing_date + ", ";
    if (!close_reason.empty()) query += " close_reason = " + close_reason + ", ";
    query += " WHERE id > 0";
    if (!id.empty()) query += " AND id = " + id;
    if (!prev_state.empty()) query += " AND state = " + prev_state;
    if (!prev_worktype.empty()) query += " AND worktype = " + prev_worktype;
    if (!customer.empty()) query += " AND customer = " + customer;
    if (!prev_worker.empty()) query += " AND worker = " + prev_worker;
    if (sqlite3_exec(db, query.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK) {
        throw Error("Unknown error did happen during request updating", DATABASE);
    }
    sqlite3_close(db);
}

ostream& operator << (ostream &out, Request request) {
    out << to_string(request.id) << "\t" << states[request.state] << "\t" <<
    worktypes[request.worktype] << "\t" << request.customer_username << "\t" <<
    request.creation_date << "\t" + request.closing_date << "\t" <<
    request.close_reason << "\n";
    return out;
}

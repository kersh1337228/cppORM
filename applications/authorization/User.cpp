#include <iostream>
#include <string>
#include <vector>
#include "sqlite3.h"
#include "../requests/Request.h"
#include "../../utils.h"
#include "../../orm/Model.h"
#include "User.h"


// User parent class methods
// ____________________________________________________________________________________________________________________
void User::check_table() {
    sqlite3* db;
    string query = "CREATE TABLE IF NOT EXISTS Users ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                   "username VARCHAR(255) NOT NULL UNIQUE, "
                   "password VARCHAR(255) NOT NULL, "
                   "role INTEGER NOT NULL);";
    sqlite3_open("db.sqlite3", &db);
    char* error_message;
    if (sqlite3_exec(db, query.c_str(), nullptr, nullptr, &error_message) != SQLITE_OK) {
        sqlite3_free(error_message);
        throw Error("Error during requests table creation", DATABASE);
    }
    sqlite3_close(db);
}

void User::sign_up(const string& username, const string& password, Role role) {
    User::check_table();
    sqlite3* db;
    sqlite3_open("db.sqlite3", &db);
    char* error_message;
    string query = "INSERT INTO Users (username, password, role) VALUES ('"
                   + username + "', '" +
                   password + "', " +
                   to_string(role) + ");";
    if (sqlite3_exec(db, query.c_str(), nullptr, nullptr, &error_message) != SQLITE_OK) {
        if (static_cast<string>(error_message).substr(0, 6) == "UNIQUE") {
            sqlite3_free(error_message);
            throw Error("User with same name already exists", AUTHENTICATION);
        } else {
            sqlite3_free(error_message);
            throw Error("Unknown error did happen during user creation", DATABASE);
        }
    }
    sqlite3_close(db);
}

User* User::sign_in(const string& username, const string& password) {
    User::check_table();
    sqlite3* db;
    sqlite3_open("db.sqlite3", &db);
    // Check if user with such name exists
    string query = "SELECT EXISTS(SELECT * FROM Users WHERE "
                   "username = '" + username + "');";
    sqlite3_exec(
        db, query.c_str(),
        [](void*, int, char** argv, char**) -> int {
            if (stoi(argv[0]) == 0)
                throw Error("No user with such name found", AUTHENTICATION);
            return 0;
        },
        nullptr, nullptr
    );
    // Selecting user
    sqlite3_stmt* stmt;
    query = "SELECT * FROM Users WHERE "
            "username = '" + username + "';";
    sqlite3_prepare_v2(db, query.c_str(), 999, &stmt, nullptr);
    sqlite3_step(stmt);
    string* row = unpack_row(stmt);
    // Check if password is valid
    if (password == row[2]) {
        User* user = new User(
            stoi(row[0]), username,
            password, static_cast<Role>(stoi(row[3]))
        );
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return user;
    } else {
        throw Error("Wrong password", AUTHENTICATION);
    }
}

void User::remove() const {
    User::check_table();
    sqlite3* db;
    sqlite3_open("db.sqlite3", &db);
    string query = "DELETE FROM Users WHERE "
                   "username = '" + this->username +
                   "' AND password = '" + this->password + "';";
    if (sqlite3_exec(db, query.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK) {
        throw Error("Error during requests table removing", DATABASE);
    }
    sqlite3_close(db);
}

void User::update_request(
    const string& requestid="",
    const string& customer="",
    const string& worker="",
    const string& state="",
    const string& worktype="",
    const string& message="",
    const string& new_worker="",
    const string& new_state="",
    const string& new_worktype="",
    const string& closing_date="",
    const string& close_reason=""
) {
    vector<Request> requests = Request::get(
        requestid, customer, worker, state, worktype
    );
    cout << "ID\tState\tType of work\tCustomer\t" <<
         "Creation date\tWorker\tClosing date\tClose reason\n";
    for (const auto& request : requests) {
        cout << request << endl;
    }
    cout << message << endl;
    int id = -2;
    while (true) {
        cin >> id;
        if (id > 0) {
            try {
                Request::update(
                    to_string(id), customer, worker, state, worktype,
                    new_worker, new_state, new_worktype, closing_date, close_reason
                );
            } catch (Error& error) {
                if (error.type == REQUEST) cerr << error.message << endl;
                else throw error;
            }
        } else if (id == -1) {
            break;
        } else {
            cout << "ID must be an unsigned integer (except -1)" << endl;
        }
    }
}


// Customer child class methods
// ____________________________________________________________________________________________________________________

void Customer::create_request(WorkType worktype)  {
    Request::create(worktype, this->id);
}

void Customer::update_request(unsigned int id, RequestState state, WorkType worktype) {
    Request::update(
        to_string(id), to_string(this->id), "", "", "",
        "", to_string(state), to_string(worktype), "", ""
    );
}

void Customer::close_request(unsigned int id) {

//    Request::close(id, "Request was canceled by client", this->id);
}

vector<Request> Customer::get_requests(
    int id=-1,
    int customer=-1,
    int worker=-1,
    RequestState state=RequestState(-1),
    WorkType worktype=WorkType(-1)
) {
    return Request::get(
        to_string(id), to_string(this->id),
        to_string(worker), to_string(state),
        to_string(worktype)
    );
}


// Manager child class methods
// ____________________________________________________________________________________________________________________
vector<Request> Manager::get_requests(
        int id=-1,
        int customer=-1,
        int worker=-1,
        RequestState state=RequestState(-1),
        WorkType worktype=WorkType(-1)
) {
    return Request::get(
            to_string(id), to_string(customer),
            to_string(worker), to_string(state),
            to_string(worktype)
    );
}


// Worker child class methods
// ____________________________________________________________________________________________________________________
vector<Request> Worker::get_requests(
        int id=-1,
        int customer=-1,
        int worker=-1,
        RequestState state=RequestState(-1),
        WorkType worktype=WorkType(-1)
) {
    vector<Request> requests = Request::get(
        to_string(id), to_string(customer),
        to_string(this->id), to_string(state),
        to_string(worktype)
    );
    vector<Request> extend = Request::get(
            to_string(id), to_string(customer),
            "", to_string(ACCEPTED),
            to_string(worktype)
    );
    requests.insert(requests.end(), extend.begin(), extend.end());
    return requests;
}

void Worker::process_request() {
    this->update_request(
        "", "", "", to_string(ACCEPTED), "",
        "Choose the request to work with",
        to_string(this->id), to_string(PROCESSING), "", "", ""
    );
}

void Worker::cancel_process_request() {
    this->update_request(
        "", "", to_string(this->id), to_string(PROCESSING), "",
        "Choose the request to cancel processing",
        "null", to_string(ACCEPTED), "", "", ""
    );
}

void Worker::confirm_process_request() {
    this->update_request(
        "", "", to_string(this->id), to_string(PROCESSING), "",
        "Choose the request to confirm that work is done",
        "", to_string(PROCESSED), "", "", ""
    );
}

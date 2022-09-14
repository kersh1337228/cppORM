#pragma once
#ifndef CARSERV_USER_H
#define CARSERV_USER_H
#include <vector>
#include "../utils.h"
#include "../requests/Request.h"

using namespace std;

class User {
protected:
    // Fields
    unsigned int id;
    string username;
    string password;
    Role role;
    // Methods
    User(unsigned int id, string username, string password, Role role):
    id(id), username(std::move(username)), password(std::move(password)), role(role) {}
    static void check_table();
    void remove() const;
    void update_request(
        const string&, const string&, const string&,
        const string&, const string&, const string&,
        const string&, const string&, const string&,
        const string&, const string&
    );
    // Request view method
    virtual vector<Request> get_requests(int, int, int, RequestState, WorkType) {};
public:
    static void sign_up(const string&, const string&, Role);
    static User* sign_in(const string&, const string&);
};

class Customer : public User {
private:
    Customer(unsigned int id, string username, string password):
    User(id, username, password, CUSTOMER) {}
public:
    // Request manage methods
    void create_request(WorkType);
    void update_request(unsigned int, RequestState, WorkType);
    void close_request(unsigned int);
    // Request view methods
    vector<Request> get_requests(int, int, int, RequestState, WorkType) final;
};

class Manager : public User {
public:
    // Request manage methods
    void set_request_state(unsigned int, RequestState);
    // Request view methods
    vector<Request> get_requests(int, int, int, RequestState, WorkType) final;
};

class Worker : public User {
public:
    void process_request();
    void cancel_process_request();
    void confirm_process_request();
    // Request view methods
    vector<Request> get_requests(int, int, int, RequestState, WorkType) final;
};

#endif

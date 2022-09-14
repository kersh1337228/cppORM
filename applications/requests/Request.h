#pragma once
#ifndef CARSERV_REQUEST_H
#define CARSERV_REQUEST_H
enum RequestState; enum WorkType;


using namespace std;

class Request {
    friend ostream& operator << (ostream&, Request);
    friend class Customer;
    friend class Manager;
    friend class User;
private:
    unsigned int id;
    RequestState state;
    WorkType worktype;
    string customer_username;
    string creation_date;
    string worker_username;
    string closing_date;
    string close_reason;
    Request(
        unsigned int id, RequestState state,
        WorkType worktype, const string& customer_username,
        const string& creation_date, const string& worker_username,
        const string& closing_date, const string& close_reason
    ): id(id), state(state), worktype(worktype),
    creation_date(creation_date),
    closing_date(closing_date),
    close_reason(close_reason),
    customer_username(customer_username),
    worker_username(worker_username) {}
public:
    static void check_table(); // Check if Requests table exists
    static void create(WorkType, unsigned int);
    static vector<Request> read(map<const string, const string&>);
    void update(
        const string& id, const string& customer,
        const string& prev_worker, const string& prev_state, const string& prev_worktype,
        const string& new_worker, const string& new_state, const string& new_worktype,
        const string& closing_date, const string& close_reason
    );
    void delet();
};

ostream& operator << (ostream &out, Request request);

#endif

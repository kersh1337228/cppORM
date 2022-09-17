#pragma once
#include "../../orm/Field.cpp"
#include "../../orm/Model.cpp"
#include <map>
#include <array>

class Request : public Model<
    std::string, std::string, int, int,
    std::chrono::system_clock::time_point,
    std::chrono::system_clock::time_point,
    std::string
> {
private:
    static std::array<std::string, 6> states;
    static std::array<std::string, 3> worktypes;
    Request(  // Value-init constructor
        std::string state, std::string worktype, int customer, int worker,
        const std::chrono::system_clock::time_point& creation_date,
        const std::chrono::system_clock::time_point& closing_date,
        const std::string& close_reason
    ): Model(state, worktype, customer, worker, creation_date, closing_date, close_reason) {};
public:
    Request(std::string state, std::string worktype, unsigned int customer): Model({
        {"state", new Field<std::string>(state)},
        {"worktype", new Field<std::string>(worktype)},
        {"customer", new Field<int>(customer)},
    }) {};
    Request(): Model(std::map<std::string, BasicField*>({
        {"state", new Field<std::string>(false, false)},
        {"worktype", new Field<std::string>(false, false)},
        {"customer", new Field<int>("customer", "Users")},
        {"worker", new Field<int>("worker", "Users")},
        {"creation_date", new Field<std::chrono::system_clock::time_point>(std::chrono::system_clock::now(), false, false)},
        {"closing_date", new Field<std::chrono::system_clock::time_point>()},
        {"close_reason", new Field<std::string>()},
    }), "Requests") {}  // Table-init constructor
    enum State { CREATED = 0, ACCEPTED = 1, PROCESSING = 2, PROCESSED = 3, PAYED = 4, CLOSED = 5 };
    enum Worktype { DIAGNOSTICS = 0, PREVENTION = 1, REPAIR = 2 };
    std::string get_state(int state) { return Request::states[state]; }
    std::string get_worktype(int worktype) { return Request::worktypes[worktype]; }
};
std::array<std::string, 6> Request::states = {"Created", "Accepted", "Processing", "Processed", "Payed", "Closed"};
std::array<std::string, 3> worktypes = {"Diagnostics", "Prevention", "Repair"};

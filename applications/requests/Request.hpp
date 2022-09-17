#pragma once
#include "../../orm/Field.hpp"
#include "../../orm/Model.hpp"
#include <map>
#include <array>

class Request : public Model<
    std::string, std::string, std::string, int, std::string, int, std::string
> {
private:
    Request(  // Value-init constructor
        std::string state, std::string worktype, int customer, int worker,
        const std::chrono::system_clock::time_point& creation_date,
        const std::chrono::system_clock::time_point& closing_date,
        const std::string& close_reason
    ): Model("Requests", {
            {"state", new Field<std::string>(state)},
            {"worktype", new Field<std::string>(worktype)},
            {"customer", new Field<int>(customer)},
            {"worker", new Field<int>(worker)},
            {"creation_date", new Field<std::chrono::system_clock::time_point>(creation_date)},
            {"closing_date", new Field<std::chrono::system_clock::time_point>(closing_date)},
            {"close_reason", new Field<std::string>(close_reason)},
    }) {};
public:
    Request(std::string state, std::string worktype, unsigned int customer): Model("Requests", {
        {"state", new Field<std::string>(state)},
        {"worktype", new Field<std::string>(worktype)},
        {"customer", new Field<int>(customer)},
    }) {};
    Request(): Model("Requests", {
        {"state", new Field<std::string>(false, false)},
        {"worktype", new Field<std::string>(false, false)},
        {"customer", new Field<int>("customer", "Users")},
        {"worker", new Field<int>("worker", "Users")},
        {"creation_date", new Field<std::chrono::system_clock::time_point>(std::chrono::system_clock::now(), false, false)},
        {"closing_date", new Field<std::chrono::system_clock::time_point>()},
        {"close_reason", new Field<std::string>()},
    }) {}  // Table-init constructor
    static std::vector<Request> read(const std::map<std::string, std::string>& data) {
        auto models = Model::read("Requests", data);
        std::vector<Request> requests;
        for (auto req : models) {
            Request request = *static_cast<Request*>(req);
            request.tbname = "Requests";
            requests.emplace_back(request);
        }
        return requests;
    }
    enum State { CREATED = 0, ACCEPTED = 1, PROCESSING = 2, PROCESSED = 3, CLOSED = 4 };
    enum Worktype { DIAGNOSTICS = 0, PREVENTION = 1, REPAIR = 2 };
    static std::array<std::string, 5> states;
    static std::array<std::string, 3> worktypes;
};
std::array<std::string, 5> Request::states = {"Created", "Accepted", "Processing", "Processed", "Closed"};
std::array<std::string, 3> Request::worktypes = {"Diagnostics", "Prevention", "Repair"};

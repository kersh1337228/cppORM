#pragma once
#ifndef CARSERV_REQUEST_H
#define CARSERV_REQUEST_H


class Request : public Model<
    RequestState,
    WorkType,
    unsigned int,
    unsigned int,
    std::chrono::system_clock::time_point,
    std::chrono::system_clock::time_point,
    std::string
> {
private:
    Request(  // Value-init constructor
        RequestState state,
        WorkType worktype,
        unsigned int customer,
        unsigned int worker,
        const std::chrono::system_clock::time_point& creation_date,
        const std::chrono::system_clock::time_point& closing_date,
        const std::string& close_reason
    ): Model(state, worktype, customer, worker, creation_date, closing_date, close_reason) {};
public:
    Request(RequestState state, WorkType worktype, unsigned int customer): Model({
        {"state", std::shared_ptr<BasicField>(new EnumField<RequestState, 6>(state))},
        {"worktype", std::shared_ptr<BasicField>(new EnumField<WorkType, 3>(worktype))},
        {"customer", std::shared_ptr<BasicField>(new IntField(customer))},
    }) {};
    Request(): Model(std::map<std::string, std::shared_ptr<BasicField>>({
        {"state", std::shared_ptr<BasicField>(new EnumField<RequestState, 6>(states))},
        {"worktype", std::shared_ptr<BasicField>(new EnumField<WorkType, 3>(worktypes))},
        {"customer", std::shared_ptr<BasicField>(new IntField("customer", "Users"))},
        {"worker", std::shared_ptr<BasicField>(new IntField("worker", "Users"))},
        {"creation_date", std::shared_ptr<BasicField>(new DateTimeField(std::chrono::system_clock::now(), false, false))},
        {"closing_date", std::shared_ptr<BasicField>(new DateTimeField())},
        {"close_reason", std::shared_ptr<BasicField>(new CharField())},
    }), "Requests") {}  // Table-init constructor
};

#endif

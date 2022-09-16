#pragma once
#ifndef CARSERV_REQUEST_H
#define CARSERV_REQUEST_H
enum RequestState; enum WorkType;


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
    ): Model(std::map<std::string, std::shared_ptr<BasicField>>({
        {"state", std::shared_ptr<BasicField>(new IntField((int)state))},
        {"worktype", std::shared_ptr<BasicField>(new IntField((int)worktype))},
        {"customer", std::shared_ptr<BasicField>(new IntField("customer", customer, "Users"))},
        {"worker", std::shared_ptr<BasicField>(new IntField("worker", worker, "Users"))},
        {"creation_date", std::shared_ptr<BasicField>(new DateTimeField(creation_date))},
        {"closing_date", std::shared_ptr<BasicField>(new DateTimeField(closing_date))},
        {"close_reason", std::shared_ptr<BasicField>(new CharField(close_reason))},
    })) {}  // Table-init constructor
public:
    Request(): Model(std::map<std::string, std::shared_ptr<BasicField>>({
        {"state", std::shared_ptr<BasicField>(new IntField())},
        {"worktype", std::shared_ptr<BasicField>(new IntField())},
        {"customer", std::shared_ptr<BasicField>(new IntField("customer", "Users"))},
        {"worker", std::shared_ptr<BasicField>(new IntField("worker", "Users"))},
        {"creation_date", std::shared_ptr<BasicField>(new DateTimeField())},
        {"closing_date", std::shared_ptr<BasicField>(new DateTimeField())},
        {"close_reason", std::shared_ptr<BasicField>(new CharField())},
    }), "Requests") {}
};

#endif

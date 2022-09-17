#include "../requests/Request.cpp"
#include <iostream>
#include <vector>

class Role {
protected:
    unsigned int userid;
    Role(unsigned int userid): userid(userid) {};
public:
    virtual std::vector<Request*> read_requests() const = 0;
    virtual constexpr std::string name() const = 0;
    enum Permisson {CUSTOMER = 0, WORKER = 1, MANAGER = 2};
};

class Customer : public Role {
public:
    Customer(unsigned int userid): Role(userid) {};
    virtual constexpr std::string name() const override {return "Customer";}
    void create_request() const {
        std::string wt; int worktype = -1;
        while (worktype < 0) {
            std::cout << "Work type (0 - Diagnostics, 1 - Prevention, 2 - Repair):" << std::endl;
            std::cin >> worktype;
            try {
                wt = Request::worktypes[worktype];
            } catch (std::exception& err) {
                std::cerr << "Wrong work type number" << std::endl;
                worktype = -1;
            }
        }  // Uniqueness check
        Request request(Request::states[Request::CREATED], wt, this->userid);
        request.create();
    }
    void update_request() const {
        // List of requests available display
        auto requests = Request::read({
            {"state", "'" + Request::states[Request::CREATED] + "' OR '" + Request::states[Request::ACCEPTED] + "'"},
            {"customer", std::to_string(this->userid)},
        });
        std::cout << requests[0]->header() << std::endl;
        for (auto request : requests) std::cout << *request << std::endl;
        // Choosing request to update
        int choice = -1;
        while (choice < 0) {
            std::cout << "Enter id of the request to update:" << std::endl;
            std::cin >> choice;
            try {
                auto req = Request::read({
                    {"id", std::to_string(choice)},
                    {"state", "'" + Request::states[Request::CREATED] + "' OR '" + Request::states[Request::ACCEPTED] + "'"},
                    {"customer", std::to_string(this->userid)},
                })[0];
                std::string wt; int worktype = -1;
                while (worktype < 0) {
                    std::cout << "Work type (0 - Diagnostics, 1 - Prevention, 2 - Repair):" << std::endl;
                    std::cin >> worktype;
                    try {
                        wt = Request::worktypes[worktype];
                    } catch (std::exception& err) {
                        std::cerr << "Wrong work type number" << std::endl;
                        worktype = -1;
                    }
                }
                req->update({
                    {"state", "'" + Request::states[Request::CREATED] + "'"},
                    {"worktype", "'" + wt + "'"}
                });
            } catch (Error& error) {
                if (error.type == MODEL){
                    std::cerr << "Wrong id" << std::endl;
                    choice = -1;
                } else throw error;
            }
        }
    }
    void cancel_request() const {
        // List of requests available display
        auto requests = Request::read({
            {"state", "'" + Request::states[Request::CREATED] + "' OR '" + Request::states[Request::ACCEPTED] + "'"},
            {"customer", std::to_string(this->userid)},
        });
        std::cout << requests[0]->header() << std::endl;
        for (auto request : requests) std::cout << *request << std::endl;
        // Choosing request to update
        int choice = -1;
        while (choice < 0) {
            std::cout << "Enter id of the request to cancel:" << std::endl;
            std::cin >> choice;
            try {
                auto req = Request::read({
                    {"id", std::to_string(choice)},
                    {"state", "'" + Request::states[Request::CREATED] + "' OR '" + Request::states[Request::ACCEPTED] + "'"},
                    {"customer", std::to_string(this->userid)},
                })[0];
                req->update({
                    {"state", "'" + Request::states[Request::CLOSED] + "'"},
                    {"closing_date", "'" + Field<std::chrono::system_clock::time_point>::tps(std::chrono::system_clock::now()) + "'"},
                    {"close_reason", "'Closed by customer'"}
                });
            } catch (Error& error) {
                if (error.type == MODEL){
                    std::cerr << "Wrong id" << std::endl;
                    choice = -1;
                } else throw error;
            }
        }
    }
    std::vector<Request*> read_requests() const final {
        auto models = Request::read({
            {"customer", std::to_string(this->userid)},
        });
        std::vector<Request*> requests;
        for (auto req : models) requests.emplace_back(static_cast<Request*>(req));
        return requests;
    }
};

class Worker : public Role {
public:
    Worker(unsigned int userid): Role(userid) {};
    virtual constexpr std::string name() const override {return "Worker";}
    void process_request() const {
        // List of requests available display
        auto requests = Request::read({{"state", "'" + Request::states[Request::ACCEPTED] + "'"}});
        std::cout << requests[0]->header() << std::endl;
        for (auto request : requests) std::cout << *request << std::endl;
        // Choosing request to update
        int choice = -1;
        while (choice < 0) {
            std::cout << "Enter id of the request to start working with:" << std::endl;
            std::cin >> choice;
            try {
                auto req = Request::read({
                    {"id", std::to_string(choice)},
                    {"state", "'" + Request::states[Request::ACCEPTED] + "'"},
                })[0];
                req->update({
                    {"state", "'" + Request::states[Request::PROCESSING] + "'"},
                    {"worker", std::to_string(this->userid)}
                });
            } catch (Error& error) {
                if (error.type == MODEL){
                    std::cerr << "Wrong id" << std::endl;
                    choice = -1;
                } else throw error;
            }
        }
    }
    void cancel_process_request() const {
        // List of requests available display
        auto requests = Request::read({
            {"state", "'" + Request::states[Request::PROCESSING] + "'"},
            {"worker", std::to_string(this->userid)}
        });
        std::cout << requests[0]->header() << std::endl;
        for (auto request : requests) std::cout << *request << std::endl;
        // Choosing request to update
        int choice = -1;
        while (choice < 0) {
            std::cout << "Enter id of the request to cancel processing of:" << std::endl;
            std::cin >> choice;
            try {
                auto req = Request::read({
                    {"id", std::to_string(choice)},
                    {"state", "'" + Request::states[Request::PROCESSING] + "'"},
                    {"worker", std::to_string(this->userid)}
                })[0];
                req->update({
                    {"state", "'" + Request::states[Request::ACCEPTED] + "'"},
                    {"worker", "NULL"}
                });
            } catch (Error& error) {
                if (error.type == MODEL){
                    std::cerr << "Wrong id" << std::endl;
                    choice = -1;
                } else throw error;
            }
        }
    }
    void confirm_request_processed() const {
        // List of requests available display
        auto requests = Request::read({
            {"state", "'" + Request::states[Request::PROCESSING] + "'"},
            {"worker", std::to_string(this->userid)}
        });
        std::cout << requests[0]->header() << std::endl;
        for (auto request : requests) std::cout << *request << std::endl;
        // Choosing request to update
        int choice = -1;
        while (choice < 0) {
            std::cout << "Enter id of the request to confirm that work is done:" << std::endl;
            std::cin >> choice;
            try {
                auto req = Request::read({
                    {"id", std::to_string(choice)},
                    {"state", "'" + Request::states[Request::PROCESSING] + "'"},
                    {"worker", std::to_string(this->userid)}
                })[0];
                req->update({{"state", "'" + Request::states[Request::PROCESSED] + "'"}});
            } catch (Error& error) {
                if (error.type == MODEL){
                    std::cerr << "Wrong id" << std::endl;
                    choice = -1;
                } else throw error;
            }
        }
    }
    std::vector<Request*> read_requests() const final {
        auto models = Request::read({{"state", "'" + Request::states[Request::ACCEPTED] + "'"}});
        auto expand = Request::read({{"worker", std::to_string(this->userid)}});
        models.insert(models.end(), expand.begin(), expand.end());
        std::vector<Request*> requests;
        for (auto req : models) requests.emplace_back(static_cast<Request*>(req));
        return requests;
    }
};

class Manager : public Role {
public:
    Manager(unsigned int userid): Role(userid) {};
    virtual constexpr std::string name() const override {return "Manager";}
    void accept_request() const {
        // List of requests available display
        auto requests = Request::read({
            {"state", "'" + Request::states[Request::CREATED] + "'"}
        });
        std::cout << requests[0]->header() << std::endl;
        for (auto request : requests) std::cout << *request << std::endl;
        // Choosing request to update
        int choice = -1;
        while (choice < 0) {
            std::cout << "Enter id of the request to continue:" << std::endl;
            std::cin >> choice;
            try {
                auto req = Request::read({
                    {"id", std::to_string(choice)},
                    {"state", "'" + Request::states[Request::CREATED] + "'"}
                })[0];
                int decision = -1;
                while (decision < 0) {
                    std::cout << " (0 - Accept, 1 - Cancel):" << std::endl;
                    std::cin >> decision;
                    switch (decision) {
                        case 0:
                            req->update({
                                {"state", "'" + Request::states[Request::ACCEPTED] + "'"}
                            });
                            break;
                        case 1: {
                            std::string reason;
                            std::cout << "Enter the reason of request canceling" << std::endl;
                            std::getline(std::cin, reason);
                            req->update({
                                {"state", "'" + Request::states[Request::CLOSED] + "'"},
                                {"closing_date", "'" + Field<std::chrono::system_clock::time_point>::tps(std::chrono::system_clock::now()) + "'"},
                                {"close_reason", "'" + reason + "'"}
                            });
                            break;
                        }
                        default:
                            std::cerr << "Wrong decision number" << std::endl;
                            decision = -1;
                    }
                }
            } catch (Error& error) {
                if (error.type == MODEL){
                    std::cerr << "Wrong id" << std::endl;
                    choice = -1;
                } else throw error;
            }
        }
    }
    void confirm_request_done() const {
        // List of requests available display
        auto requests = Request::read({{"state", "'" + Request::states[Request::PROCESSED] + "'"}});
        std::cout << requests[0]->header() << std::endl;
        for (auto request : requests) std::cout << *request << std::endl;
        // Choosing request to update
        int choice = -1;
        while (choice < 0) {
            std::cout << "Enter id of the request to confirm everything is done:" << std::endl;
            std::cin >> choice;
            try {
                auto req = Request::read({
                    {"id", std::to_string(choice)},
                    {"state", "'" + Request::states[Request::PROCESSED] + "'"},
                })[0];
                req->update({
                    {"state", "'" + Request::states[Request::CLOSED] + "'"},
                    {"closing_date", "'" + Field<std::chrono::system_clock::time_point>::tps(std::chrono::system_clock::now()) + "'"},
                    {"close_reason", "'The work was successfully done'"}
                });
            } catch (Error& error) {
                if (error.type == MODEL){
                    std::cerr << "Wrong id" << std::endl;
                    choice = -1;
                } else throw error;
            }
        }
    }
    std::vector<Request*> read_requests() const final {
        auto models = Request::read({});
        std::vector<Request*> requests;
        for (auto req : models) requests.emplace_back(static_cast<Request*>(req));
        return requests;
    }
};

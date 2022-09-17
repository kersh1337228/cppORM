#include "../requests/Request.cpp"
#include <iostream>
#include <vector>

class Role {
protected:
    Role() = default;
public:
    virtual constexpr std::vector<Request*> read_requests() const = 0;
    virtual constexpr std::string name() const = 0;
    enum Permisson {CUSTOMER = 0, WORKER = 1, MANAGER = 2};
};

class Customer : public Role {
public:
    Customer() = default;
    virtual constexpr std::string name() const override {return "Customer";}
    void create_request(unsigned int userid) const {
        std::string wt; int worktype = -1;
        while (worktype < 0) {
            std::cout << "Work type (0 - Diagnostics, 1 - Prevention, 2 - Repair):" << std::endl;
            std::cin >> worktype;
            switch (worktype) {
                case Request::DIAGNOSTICS: wt = "Diagnostics"; break;
                case Request::PREVENTION: wt = "Prevention"; break;
                case Request::REPAIR: wt = "Repair"; break;
                default:
                    std::cerr << "Wrong work type number" << std::endl;
                    worktype = -1;
            }
        }  // Uniqueness check
        Request request("Created", wt, userid);
        request.create();
    }
    void update_request(unsigned int userid) const {
        // List of requests available display
        auto requests = Request::read({
            {"state", "'Created' OR 'Accepted'"},
            {"customer", std::to_string(userid)},
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
                    {"state", "'Created' OR 'Accepted'"},
                    {"customer", std::to_string(userid)},
                })[0];
                std::string wt; int worktype = -1;
                while (worktype < 0) {
                    std::cout << "Work type (0 - Diagnostics, 1 - Prevention, 2 - Repair):" << std::endl;
                    std::cin >> worktype;
                    switch (worktype) {
                        case Request::DIAGNOSTICS: wt = "Diagnostics"; break;
                        case Request::PREVENTION: wt = "Prevention"; break;
                        case Request::REPAIR: wt = "Repair"; break;
                        default:
                            std::cerr << "Wrong work type number" << std::endl;
                            worktype = -1;
                    }
                }  // Uniqueness check
                req->update({{"worktype", wt}});
            } catch (Error& error) {
                if (error.type == MODEL){
                    std::cerr << "Wrong id" << std::endl;
                    choice = -1;
                } else throw error;
            }
        }
    }
    void close_request() const;
    constexpr std::vector<Request*> read_requests() const final;
};

class Worker : public Role {
public:
    Worker() = default;
    virtual constexpr std::string name() const override {return "Worker";}
    void process_request() const;
    void cancel_process_request() const;
    void confirm_request_processed() const;
    constexpr std::vector<Request*> read_requests() const final;
};

class Manager : public Role {
public:
    Manager() = default;
    virtual constexpr std::string name() const override {return "Manager";}
    void accept_request() const;
    void cancel_request() const ;
    void confirm_request_payment() const;
    void confirm_request_done() const;
    constexpr std::vector<Request*> read_requests() const final;
};

#ifndef CARSERV_ROLE_H
#define CARSERV_ROLE_H

enum Roles {
    CUSTOMER = 0,
    WORKER = 1,
    MANAGER = 2,
};

const std::array<std::string, 3> roles = {
    "Customer", "Worker", "Manager"
};

class Role {
protected:
    Roles id;
    Role(Roles id): id(id) {};
public:
    virtual constexpr std::vector<std::unique_ptr<Request>> read_requests() const = 0;
};

class Customer : public Role {
public:
    Customer(): Role(CUSTOMER) {};
    void create_request(unsigned int userid) const;
    void update_request() const;
    void close_request() const;
    constexpr std::vector<std::unique_ptr<Request>> read_requests() const final;
};

class Worker : public Role {
public:
    Worker(): Role(WORKER) {};
    void process_request() const;
    void cancel_process_request() const;
    void confirm_request_processed() const;
    constexpr std::vector<std::unique_ptr<Request>> read_requests() const final;
};

class Manager : public Role {
public:
    Manager(): Role(MANAGER) {};
    void accept_request() const;
    void cancel_request() const ;
    void confirm_request_payment() const;
    void confirm_request_done() const;
    constexpr std::vector<std::unique_ptr<Request>> read_requests() const final;
};

#endif

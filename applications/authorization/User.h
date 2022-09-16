#pragma once
#ifndef CARSERV_USER_H
#define CARSERV_USER_H

class User : public Model<
    std::string,
    std::string,
    Roles
> {
public:
    User(const std::string& username, const std::string& password, Roles role):
    Model(username, password, role) {}; // Value-init constructor
    User(): Model(std::map<std::string, std::shared_ptr<BasicField>>({
         {"username", std::shared_ptr<BasicField>(new CharField(false, true))},
         {"password", std::shared_ptr<BasicField>(new CharField(false, false))},
         {"role", std::shared_ptr<BasicField>(new EnumField<Roles, 3>(roles))},
    }), "Users") {}  // Table-init constructor
};

User sign_up();
User sign_in();

#endif

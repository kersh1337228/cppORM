#pragma once
#include "../orm/Model.hpp"
#include "Role.hpp"
#include <iostream>

class User : public Model<std::string, int, std::string> {
public:
    Role* role;
    User(const std::string& username, const std::string& password, int role):
    Model("Users", {
        {"password", new Field<std::string>(password)},
        {"role", new Field<int>(role)},
        {"username", new Field<std::string>(username)}
    }) {
        switch (role) {
            case Role::CUSTOMER: this->role = new Customer(this->id); break;
            case Role::WORKER: this->role = new Worker(this->id); break;
            case Role::MANAGER: this->role = new Manager(this->id); break;
            default: this->role = new Customer(this->id); break;
        }
    }; // Value-init constructor
    User(): Model("Users", {
         {"password", new Field<std::string>(false, false)},
         {"role", new Field<int>()},
         {"username", new Field<std::string>(false, true)}
    }) {}  // Table-init constructor
    static std::vector<User> read(const std::map<std::string, std::string>& data) {
        auto models = Model::read("Users", data);
        std::vector<User> users;
        for (auto usr : models) {
            User user = *static_cast<User*>(usr);
            int role = static_cast<Field<int>*>(user.fields["role"])->value;
            switch (role) {
                case Role::CUSTOMER: user.role = new Customer(user.id); break;
                case Role::WORKER:user.role  = new Worker(user.id); break;
                case Role::MANAGER: user.role  = new Manager(user.id); break;
            }
            user.tbname = "Users";
            users.emplace_back(user);
        }
        return users;
    }
    static User sign_up() {
        // Interactive input interface
        std::string username, password;
        int role = -1;
        std::cout << "Username:" << std::endl;
        std::cin.ignore();
        std::getline(std::cin, username);
        std::cout << "Password:" << std::endl;
        std::getline(std::cin, password);
        std::cout << "Role (0 - Customer, 1 - Worker, 2 - Manager):" << std::endl;
        std::cin >> role;
        auto users = User::read({{"username", "'" + username + "'"}});
        if (users.size() > 0) throw Error("User with same name already exists", MODEL);
        User user(username, password, role);
        user.create();  // Creating database row
        return user;
    }
    static User sign_in() {
        // Interactive input interface
        std::string username, password;
        std::cout << "Username:" << std::endl;
        std::cin.ignore();
        std::getline(std::cin, username);
        std::cout << "Password:" << std::endl;
        std::getline(std::cin, password);
        auto users = User::read({
            {"username", "'" + username + "'"},
            {"password", "'" + password + "'"}
        });
        return users[0];
    }
};

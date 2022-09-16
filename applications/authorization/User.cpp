#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <array>
#include "../../orm/Field.h"
#include "../../orm/Model.h"
#include "../../utils.h"
#include "Role.h"
#include "User.h"

User sign_up() {
    // Interactive input interface
    std::string username, password;
    Roles role; int temp_role = -1;
    std::cout << "Username:" << std::endl;
    std::getline(std::cin, username);
    std::cout << "Password:" << std::endl;
    std::getline(std::cin, password);
    while (temp_role < 0) {
        std::cout << "Role (0 - Customer, 1 - Worker, 2 - Manager):" << std::endl;
        std::cin >> temp_role;
        switch (temp_role) {
            case 0:
                role = CUSTOMER;
                break;
            case 1:
                role = WORKER;
                break;
            case 2:
                role = MANAGER;
                break;
            default:
                std::cerr << "Wrong role number" << std::endl;
                temp_role = -1;
        }
    }  // Uniqueness check
    auto users = User::read({{"username", "'" + username + "'"}});
    if (users.size() > 0) throw Error("User with same name already exists", MODEL);
    User user(username, password, role);
    user.create();  // Creating database row
    return user;
}
User sign_in() {
    // Interactive input interface
    std::string username, password;
    std::cout << "Username:" << std::endl;
    std::getline(std::cin, username);
    std::cout << "Password:" << std::endl;
    std::getline(std::cin, password);
    auto users = User::read({
        {"username", "'" + username + "'"},
        {"password", "'" + password + "'"}
    });
    User* user = static_cast<User*>(users[0].get());
    return *user;
}

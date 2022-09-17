#include "../orm/Model.cpp"
#include "Role.cpp"
#include <iostream>

class User : public Model<std::string, std::string, int> {
public:
    Role* role;
    User(const std::string& username, const std::string& password, int role):
    Model(username, password, role) {
        switch (role) {
            case Role::CUSTOMER: this->role = new Customer(); break;
            case Role::WORKER: this->role = new Worker(); break;
            case Role::MANAGER: this->role = new Manager(); break;
            default: this->role = new Customer(); break;
        }
    }; // Value-init constructor
    User(): Model(std::map<std::string, BasicField*>({
         {"username", new Field<std::string>(false, true)},
         {"password", new Field<std::string>(false, false)},
         {"role", new Field<int>()},
    }), "Users") {}  // Table-init constructor
};

User sign_up() {
    // Interactive input interface
    std::string username, password;
    int role = -1;
    std::cout << "Username:" << std::endl;
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
    User* user = static_cast<User*>(users[0]);
    return *user;
}

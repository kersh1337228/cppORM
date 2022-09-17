#include "utils.hpp"
#include "orm/Field.hpp"
#include "orm/Model.hpp"
#include "applications/requests/Request.hpp"
#include "applications/authorization/Role.hpp"
#include "applications/authorization/User.hpp"
#include <fstream>

void entry_point() {
    // Authentication
    while (true) {
        try {
            int choice = -1;
            std::cout << "Welcome to the system."
                         " If you already have an account sign in (0), please."
                         " In case you do not own one sign up (1). To exit enter 2.\n";
            User user;
            while (choice < 0) {
                std::cin >> choice;
                switch (choice) {
                    case 0: user = User::sign_in(); break;
                    case 1: user = User::sign_up(); break;
                    case 2: exit(0);
                    default: std::cerr << "Wrong choice" << std::endl; choice = -1;
                }
            }
            // Working process
            bool signed_in = true;
            while(signed_in) {
                std::cout << "Choose one of the next options:" << std::endl;
                choice = -1;
                if (user.role->name() == "Customer") {
                    Customer role = *dynamic_cast<Customer*>(user.role);
                    std::cout << "1. Create request;\n"
                                 "2. Alter request\n"
                                 "3. Cancel request\n"
                                 "4. View requests\n"
                                 "5. Sign out\n";
                    std::cerr << "6. Delete account\n";
                    while (choice < 0) {
                        std::cin >> choice;
                        switch (choice) {
                            case 1: role.create_request(); break;
                            case 2: role.update_request(); break;
                            case 3: role.cancel_request(); break;
                            case 4: role.read_requests(); break;
                            case 5: signed_in = false; break;
                            case 6: user.remove(); signed_in = false; break;
                            default:
                                std::cerr << "Wrong choice" << std::endl;
                                choice = -1;
                        }
                    }
                } else if (user.role->name() == "Worker") {
                    Worker role = *dynamic_cast<Worker*>(user.role);
                    std::cout << "1. Start request processing;\n"
                                 "2. Cancel request processing\n"
                                 "3. Confirm request processing\n"
                                 "4. View requests\n"
                                 "5. Sign out\n";
                    std::cerr << "6. Delete account\n";
                    while (choice < 0) {
                        std::cin >> choice;
                        switch (choice) {
                            case 1: role.process_request(); break;
                            case 2: role.cancel_process_request(); break;
                            case 3: role.confirm_request_processed(); break;
                            case 4: role.read_requests(); break;
                            case 5: signed_in = false; break;
                            case 6: user.remove(); signed_in = false; break;
                            default:
                                std::cerr << "Wrong choice" << std::endl;
                                choice = -1;
                        }
                    }
                } else {
                    Manager role = *dynamic_cast<Manager*>(user.role);
                    std::cout << "1. Accept or close request;\n"
                                 "2. Confirm request done\n"
                                 "3. View requests"
                                 "4. Sign out\n";
                    std::cerr << "5. Delete account\n";
                    while (choice < 0) {
                        std::cin >> choice;
                        switch (choice) {
                            case 1: role.accept_request(); break;
                            case 2: role.confirm_request_done(); break;
                            case 3: role.read_requests(); break;
                            case 4: signed_in = false; break;
                            case 5: user.remove(); signed_in = false; break;
                            default:
                                std::cerr << "Wrong choice" << std::endl;
                                choice = -1;
                        }
                    }
                }
            }
        } catch (Error& error) {
            std::cerr << error.message << std::endl;
        }
    }
}

int main() {
//     Creating database if not exists
    std::fstream db_file;
    db_file.open("db.sqlite3", std::ios::_Noreplace | std::ios::app);
    if(db_file.is_open()) db_file.close();
    // System execution
    entry_point();
}

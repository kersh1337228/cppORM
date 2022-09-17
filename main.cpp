#include "applications/authorization/User.cpp"
#include "applications/authorization/Role.cpp"
#include "utils.cpp"
#include <iostream>
#include <fstream>

void entry_point() {
    int choice = -1;
    std::cout << "Welcome to the system."
    " If you already have an account sign in (0), please."
    " In case you do not own one sign up (1)" << std::endl;
    User user;
    while (choice < 0) {
        switch (choice) {
            case 0: user = sign_in(); break;
            case 1: user = sign_up(); break;
            default: std::cerr << "Wrong choice" << std::endl; choice = -1;
        }
    }
}

int main() {
    // Check if database file exists
    std::fstream db_file;
    db_file.open("db.sqlite3", std::ios::_Noreplace | std::ios::app);
    if(db_file.is_open()) db_file.close();
    User(); Request(); // Creating tables
    try {
        entry_point();
    } catch (Error& error) {
        std::cerr << error.message << std::endl;
    }
}

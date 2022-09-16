#include <iostream>
#include <memory>
#include <vector>
#include <array>
#include "../../utils.h"
#include "../orm/Model.h"
#include "../requests/Request.h"
#include "Role.h"

void Customer::create_request(unsigned int userid) const {
    WorkType worktype; int temp_worktype = -1;
    while (temp_worktype < 0) {
        std::cout << "Role (0 - Customer, 1 - Worker, 2 - Manager):" << std::endl;
        std::cin >> temp_worktype;
        switch (temp_worktype) {
            case 0:
                worktype = DIAGNOSTICS;
                break;
            case 1:
                worktype = PREVENTION;
                break;
            case 2:
                worktype = REPAIR;
                break;
            default:
                std::cerr << "Wrong work type number" << std::endl;
                temp_worktype = -1;
        }
    }  // Uniqueness check
    Request request(CREATED, worktype, userid);
    request.create();
};

cmake_minimum_required(VERSION 3.22)
project(carserv)

set(CMAKE_CXX_STANDARD 23)

include_directories(${CMAKE_SOURCE_DIR}/sqlite3)
add_library(sqlite3 STATIC sqlite3/sqlite3.c sqlite3/sqlite3.h sqlite3/sqlite3ext.h)
add_executable(carserv main.cpp applications/authorization/User.cpp applications/authorization/User.h applications/requests/Request.cpp applications/requests/Request.h utils.h utils.cpp utils.h orm/Model.cpp orm/Model.h orm/Field.cpp orm/Field.h)
target_link_libraries(carserv LINK_PUBLIC sqlite3)

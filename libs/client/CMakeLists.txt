add_library(client client.cpp)

find_package(Boost REQUIRED COMPONENTS headers)

target_link_libraries(client Boost::headers)
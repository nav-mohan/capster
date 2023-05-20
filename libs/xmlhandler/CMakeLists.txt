add_library(xmlhandler xmlhandler.cpp)

find_package(Boost REQUIRED COMPONENTS headers)

target_link_libraries(xmlhandler Boost::headers )




    

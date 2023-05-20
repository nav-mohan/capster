cmake_minimum_required(VERSION 3.11)
project(Capster LANGUAGES CXX C)
set (CMAKE_CXX_STANDARD 17)

set(TARGET_NAME capster)

add_subdirectory(libs/client)
add_subdirectory(libs/xmlhandler)
add_subdirectory(libs/audioutil)
add_subdirectory(libs/rs232util)
add_subdirectory(libs/geoutil)

add_executable(${TARGET_NAME} main.cpp)
target_include_directories(capster PUBLIC libs/client libs/xmlhandler libs/audioutil libs/rs232util libs/geoutil)
target_link_libraries(${TARGET_NAME} client xmlhandler audioutil rs232util geoutil)

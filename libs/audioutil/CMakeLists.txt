add_library(audioutil audioutil.cpp)


find_package(CURL REQUIRED)

target_include_directories(
    audioutil 
    PUBLIC 
    ${CURL_INCLUDE_DIR}
    /usr/local/Cellar/mpg123/1.31.3/include
)

message("CURL INCLUDE DIR " ${CURL_INCLUDE_DIR})

target_link_libraries(
    audioutil 
    ${CURL_LIBRARIES}
    /usr/local/Cellar/mpg123/1.31.3/lib/libmpg123.a
    /usr/local/Cellar/mpg123/1.31.3/lib/libout123.a
)




message("CURL lib dir " ${CURL_LIBRARIES})
    

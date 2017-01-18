cmake_minimum_required(VERSION 3.6.2)

project(database-final)

# Enable unit test
option(test "Build all tests." ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Enable debugging
add_compile_options(-g)


# Remember to add one file per line
# so that git can track file change correctly.
set(SOURCE
    src/bpt.cpp
    src/action.cpp
    src/util.cpp
    src/page.cpp
    src/value.cpp
    src/rm.cpp
)

add_library(database-final ${SOURCE})

add_executable(main src/main.cpp)
target_link_libraries(main database-final)

target_include_directories(database-final PUBLIC src)

if(test)
    # Enable gtest and gmock
    enable_testing()
    add_subdirectory(test)
endif()

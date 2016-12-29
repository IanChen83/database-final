add_executable(testbpt testbpt.cpp)
target_include_directories(testbpt PUBLIC ../../src)
target_link_libraries(testbpt libgtest database-final)

add_test(testbpt testbpt)

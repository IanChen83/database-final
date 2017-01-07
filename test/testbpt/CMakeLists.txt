add_executable(testbpt
    main.cpp
    test_value.cpp
    testbpt.cpp)
target_include_directories(testbpt PUBLIC ../../src)
target_link_libraries(testbpt libgtest database-final)

add_test(testbpt testbpt)

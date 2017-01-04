add_executable(testspec
    main.cpp
    action_spec.cpp
    action_usage.cpp
    )

target_include_directories(testspec PUBLIC ../../src)
target_link_libraries(testspec libgtest database-final)

add_test(testspec testspec)

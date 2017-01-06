add_executable(testpage
    main.cpp
    testspec.cpp
    )

target_include_directories(testpage PUBLIC ../../src)
target_link_libraries(testpage libgtest database-final)

add_test(testpage testpage)

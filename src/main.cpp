#include <iostream>
#include "bpt.h"

int main(int argc, char* argv[]) {
    auto hello_string = "Hello world!";
    std::cout << hello_string << init_test() << std::endl;
    return init_test();
}

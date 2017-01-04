#include "action.h"
#include "util.h"
#include <string>
#include <iostream>

using namespace std;

bool
isValidRInput(const char* input) {
    if(input == NULL || input[0] != 'R') {
        return false;
    }

    auto tokens = tokenize(input, ',', true);

    if(tokens.size() != 4 || tokens[0] != "R" || tokens[1] == "") {
        return false;
    }

    if(
            tokens[2] != "Integer" &&
            tokens[2] != "integer" &&
            tokens[2] != "String" &&
            tokens[2] != "string"
      )
        return false;

    try {
        if(std::stoi(tokens[3]) == 0)
            return false;
    } catch(const std::exception& e) {
        return false;
    }

    return true;
}
bool
isValidIInput(const char* input) {
    if(input == NULL || input[0] != 'I')
        return false;

    auto records = tokenize(input, ';', true);
    auto tokens = tokenize(records[0], ',', true);

    if(
            tokens.size() != 4 ||
            tokens[0] != "I" ||
            tokens[1] == "" ||
            tokens[2] == "" ||
            tokens[3] == ""
    )
        return false;

    records[0] = tokens[2] + "," + tokens[3];

    for(int i = 1; i < records.size(); ++i) {
        auto x = tokenize(records[i], ',', true);
        if(x.size() != 2 || x[0] == "" || x[1] == "") {
            return false;
        }
    }

    return true;
}
bool
isValidDInput(const char* input) {
    if(input == NULL || input[0] != 'D')
        return false;

    auto tokens = tokenize(input, ',', true);

    if(tokens.size() != 3 || tokens[0] != "D" || tokens[1] == "" || tokens[2] == "")
        return false;

    if(tokens[2][0] != '"' || tokens[2][tokens[2].length() - 1] != '"')
        return false;

    return true;
}

bool
isValidInput(const char* input) {
    if(input == NULL) {
        return false;
    }

    if(input[0] == 'R') {
        return isValidRInput(input);
    } else if(input[0] == 'I') {
        return isValidIInput(input);
    } else if(input[0] == 'D') {
        return isValidDInput(input);
    }
}

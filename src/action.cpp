#include "action.h"
#include "util.h"
#include <algorithm>
#include <assert.h>
#include <tuple>
#include <string>
#include <cstring>
#include <iostream>

using namespace std;

Record*
createRecord(int v, const char* d) {
    // return new Record(new Value(v), d);
    return NULL;
}

Record*
createRecord(const char* v, const char* d) {
    // return new Record(new Value(v), d);
    return NULL;
}

Value*
getValue(string input) {
    if(input[0] == '"')
        return createValue(input.substr(1, input.size() - 2).c_str());
    else
        return createValue(std::stoi(input));
}

static inline void
removeRecord(Record* v) {
    delete std::get<0>(*v);
}

bool
isValidRecord(const char* input) {
    if(
            input == NULL ||
            strlen(input) <= 2 ||
            input[0] != '"' ||
            input[strlen(input) - 1] != '"'
      )
        return false;

    for(const char* i = input + strlen(input) - 2; i > input ; --i) {
        if(*i == '"') {
            return false;
        }
    }

    return true;
}

bool
isValidValue(const char* input) {
    if(input == NULL)
        return false;
    const string r(input);
    if(input[0] == '"' && input[strlen(input) - 1] == '"') {
        for(const char* i = input + strlen(input) - 2; i > input ; --i) {
            if(*i == '"') {
                return false;
            }
        }
        return true;
    } else {
        try{
            if(std::stoi(r) || std::stoi(r) == 0)
                return true;
        } catch(const exception& e) {
        }
        return false;
    }
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
    } else if(input[0] == 'S') {
        return isValidSInput(input);
    } else if(input[0] == 'q') {
        return isValidQInput(input);
    } else if(input[0] == 'p') {
        return isValidPInput(input);
    } else if(input[0] == 'c') {
        return isValidCInput(input);
    }

    return false;
}

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
        if(std::stoi(tokens[3]) <= 0)
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

    auto records = tokenizeEscape(input, ';', true);
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

    for(auto& record: records) {
        auto x = tokenize(record, ',', true);

        if(x.size() != 2 || x[0] == "" || x[1] == "") {
            return false;
        }
        if(!isValidValue(x[0].c_str()) || !isValidRecord(x[1].c_str()))
            return false;
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

    if(!isValidValue(tokens[2].c_str()))
        return false;

    return true;
}

bool
isValidSInput(const char* input) {
    if(input == NULL || input[0] != 'S')
        return false;

    auto tokens = tokenize(input, ' ', true);

    if(tokens.size() != 2 || tokens[0] != "Scan")
        return false;

    return true;
}

bool
isValidQInput(const char* input) {
    if(input == NULL || input[0] != 'q')
        return false;

    auto tokens = tokenize(input, ' ', true);

    if((tokens.size() != 3 && tokens.size() != 4) || tokens[0] != "q")
        return false;

    if(tokens.size() == 3)
        return isValidValue(tokens[2].c_str());

    return (isValidValue(tokens[2].c_str()) && isValidValue(tokens[3].c_str()));
}
bool
isValidPInput(const char* input) {
    if(input == NULL || input[0] != 'p')
        return false;

    auto tokens = tokenize(input, ' ', true);

    if(tokens.size() != 3 || tokens[0] != "p")
        return false;

    try{
        if(std::stoi(tokens[2]) >= 0 && tokens[2].find('.') == string::npos)
            return true;
    } catch(const exception& e) {
    }

    return false;
}

bool
isValidCInput(const char* input) {
    if(input == NULL || input[0] != 'c')
        return false;

    auto tokens = tokenize(input, ' ', true);

    if(tokens.size() != 2 || tokens[0] != "c")
        return false;

    return true;
}

RPayload
getRPayload(const char* input) {
    auto tokens = tokenize(input, ',', true);
    RPayload r = {
        .name = tokens[1].c_str(),
        .type = (tokens[2] == "String" || tokens[2] == "String") ?
            ValueType::String : ValueType::Integer,
        .size = (unsigned int)std::stoi(tokens[3])
    };
    return r;
}

IPayload
getIPayload(const char* input) {
    auto records = tokenizeEscape(input, ';', true);
    auto tokens = tokenize(records[0], ',', true);
    records[0] = tokens[2] + "," + tokens[3];

    vector<Record> values;

    for(auto& x: records) {
        auto token = tokenize(x.c_str(), ',', true);
        auto value = getValue(token[0]);
        values.emplace_back(value, token[1].substr(1, token[1].size() - 2));
    }

    IPayload r = {
        .name = tokens[1],
        .values = values
    };

    return r;
}

DPayload
getDPayload(const char* input) {
    auto tokens = tokenize(input, ',', true);
    DPayload r = {
        .name = tokens[1],
        .value = createValue(tokens[2].c_str())
    };
    return r;
}

SPayload
getSPayload(const char* input) {
    auto tokens = tokenize(input, ' ', true);
    SPayload r = {
        .name = tokens[1]
    };
    return r;
}

QPayload
getQPayload(const char* input) {
    auto tokens = tokenize(input, ' ', true);

    Value* value1 = NULL;
    Value* value2 = NULL;

    if(tokens.size() == 3) {
        value1 = getValue(tokens[2]);
    } else {
        value1 = getValue(tokens[2]);
        value2 = getValue(tokens[3]);
    }

    QPayload r = {
        .name = tokens[1],
        .value1 = value1,
        .value2 = value2
    };

    return r;
}

PPayload
getPPayload(const char* input) {
    auto tokens = tokenize(input, ' ', true);
    PPayload r = {
        .name = tokens[1],
        .pid = (uint16_t)std::stoi(tokens[2])
    };

    return r;
}

CPayload
getCPayload(const char* input) {
    auto tokens = tokenize(input, ' ', true);
    CPayload r = {
        .name = tokens[1]
    };

    return r;
}

Action*
getAction(const char* input) {
    if(input == NULL || !isValidInput(input))
        return NULL;

    if(input[0] == 'R')
        return new Action(getRPayload(input));
    else if(input[0] == 'I')
        return new Action(getIPayload(input));
    else if(input[0] == 'D')
        return new Action(getDPayload(input));
    else if(input[0] == 'S')
        return new Action(getSPayload(input));
    else if(input[0] == 'q')
        return new Action(getQPayload(input));
    else if(input[0] == 'p')
        return new Action(getPPayload(input));
    else if(input[0] == 'c')
        return new Action(getCPayload(input));

    return NULL;
}

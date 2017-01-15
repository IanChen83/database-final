#include "action.h"
#include "util.h"
#include <algorithm>
#include <string>
#include <cstring>
#include <iostream>

using namespace std;

bool
operator<(const Value &l, const Value &r) {
    if (l.type == ValueType::Integer) {
        return l.IntValue < r.IntValue;
    } else if (l.type == ValueType::String) {
        return strcmp(l.StrValue, r.StrValue) < 0;
    }
}


Value*
createValue(int v, size_t size) {
    size = max(sizeof(Value), size);
    char* r = new char[size];
    ((Value*)r)->type = ValueType::Integer;
    ((Value*)r)->IntValue = v;
    return (Value*)r;
}

Value*
createValue(const char* v, size_t size) {
    size = max(sizeof(Value) + strlen(v), size);
    char* r = new char[size];
    ((Value*)r)->type = ValueType::String;
    ((Value*)r)->StrValue = (char*)((Value*)r + 1);
    strcpy((char*)((Value*)r + 1), v);
    return (Value*)r;
}

static inline void
removeValue(Value* v) {
    delete[] (char*)v;
}

Record*
createRecord(int v, const char* d) {
    return NULL;
}

Record*
createRecord(const char* v, const char* d) {
    return NULL;
}

static inline void
removeRecord(Record* v) {
    delete[] v;
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
isValidCInput(const char* input) {
    if(input == NULL || input[0] != 'c')
        return false;

    auto tokens = tokenize(input, ' ', true);

    if(tokens.size() != 2 || tokens[0] != "c")
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
    } else if(input[0] == 'S') {
        return isValidSInput(input);
    } else if(input[0] == 'c') {
        return isValidCInput(input);
    }

    return false;
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
        char* recordValue = new char[token[1].size() - 1];
        strcpy(recordValue, token[1].substr(1, token[1].size() - 2).c_str());
        if(token[0][0] == '"') {
            values.emplace_back(
                    createValue(token[0].substr(1, token[0].size() - 2).c_str()),
                    recordValue
                );
        } else {
            values.emplace_back(
                    createValue(std::stoi(token[0])),
                    recordValue
                );
        }
    }

    IPayload r = {
        .name = tokens[1].c_str(),
        .values = values
    };

    return r;
}

DPayload
getDPayload(const char* input) {
    auto tokens = tokenize(input, ',', true);
    DPayload r = {
        .name = tokens[1].c_str(),
        .value = createValue(tokens[2].c_str())
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
}

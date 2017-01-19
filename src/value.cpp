#include "value.h"
#include <assert.h>
#include <string>
#include <cstring>

using namespace std;

bool
operator>(const Value &l, const Value &r) {
    if (l.type == ValueType::Integer) {
        return l.IntValue > r.IntValue;
    } else if (l.type == ValueType::String) {
        return strcmp(l.StrValue, r.StrValue) > 0;
    } else {
        assert(0);
    }
}

bool
operator<(const Value &l, const Value &r) {
    if (l.type == ValueType::Integer) {
        return l.IntValue < r.IntValue;
    } else if (l.type == ValueType::String) {
        return strcmp(l.StrValue, r.StrValue) < 0;
    } else {
        assert(0);
    }
}

bool
operator<=(const Value &l, const Value &r) {
    if (l.type == ValueType::Integer) {
        return l.IntValue <= r.IntValue;
    } else if (l.type == ValueType::String) {
        return strcmp(l.StrValue, r.StrValue) <= 0;
    } else {
        assert(0);
    }
}

bool
operator!=(const Value &l, const Value &r) {
    if (l.type == ValueType::Integer) {
        return l.IntValue != r.IntValue;
    } else if (l.type == ValueType::String) {
        return strcmp(l.StrValue, r.StrValue) != 0;
    } else {
        assert(0);
    }
}

bool
operator==(const Value &l, const Value &r) {
    if (l.type == ValueType::Integer) {
        return l.IntValue == r.IntValue;
    } else if (l.type == ValueType::String) {
        return strcmp(l.StrValue, r.StrValue) == 0;
    } else {
        assert(0);
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
    size = max(sizeof(Value) + strlen(v) + 1, size);
    char* r = new char[size]();
    ((Value*)r)->type = ValueType::String;
    ((Value*)r)->StrValue = (char*)((Value*)r + 1);
    strcpy((char*)((Value*)r + 1), v);
    return (Value*)r;
}

string
getValueStr(const Value& v) {
    if (v.type == ValueType::Integer) {
        return to_string(v.IntValue);
    } else if (v.type == ValueType::String) {
        return string(v.StrValue);
    } else {
        assert(0);
    }
}

void
removeValue(Value* v) {
    delete[] (char*)v;
}

#ifndef VALUE_HEADER
#define VALUE_HEADER

#include <string>
#include <utility>
/*
 * There are two types of Value: Integer and String
 *
 * An Value is defined by ValueType/ValueData pair
 *
 * Although it is called 'Value', this struct is used to
 * store key of each record
 *
 * */
enum ValueType { Undefined, Integer, String };

struct Value {
    Value(): type(ValueType::Undefined), IntValue(0) {}
    Value(int v): type(ValueType::Integer), IntValue(v) {}
    Value(char* v): type(ValueType::String), StrValue(v) {}
    ValueType type;
    union {
        int IntValue;
        char* StrValue;
    };
};
bool operator> (const Value&, const Value&);
bool operator< (const Value&, const Value&);
bool operator<= (const Value&, const Value&);
bool operator!= (const Value&, const Value&);
bool operator== (const Value&, const Value&);


/*
 * Because key(index) will be potentially be stored centralized.
 * To minimize copy/move cost, key should be created in heap and
 * access via pointer.
 *
 * These functions return a Value in heap, and the data (IntValue
 * or StrValue) are also stored in heap.
 *
 * Call removeValue if the Value is to be removed (or memory leak
 * will occur).
 *
 * function signature:
 * Value* createValue(data, min_size)
 *
 * */
Value* createValue(int, size_t = 0);
Value* createValue(const char*, size_t = 0);

void removeValue(Value*);

#endif

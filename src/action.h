#ifndef ACTION_HEADER
#define ACTION_HEADER

#include <string>
#include <vector>
/*
 * There are three types of action:
 *
 * R: Define a new relation
 *      R, Relation-name, key-type, record-length
 *
 * I: Insert one or many records into an existing relation
 *      I, Relation-name, [key-value, record-value(s)]+
 *
 * D: Delete a record by its key
 *      D, Relation-name, key-value
 *
 * U: Unknown action type Implementation defined
 * */
enum ActionType { U, R, I, D };


/*
 * An Value is defined by ValueType/ValueData pair
 *
 * */
enum ValueType { Undefined, Integer, String };

struct Value {
    Value(): type(ValueType::Undefined), IntValue(0), n(NULL) {}
    Value(int v, Value* _n = NULL): type(ValueType::Integer), IntValue(v), n(_n) {}
    Value(char* v, Value* _n = NULL): type(ValueType::String), StrValue(v), n(_n) {}
    ValueType type;
    union {
        int IntValue;
        char* StrValue;
    };
    Value* n;
};

/*
 * An input data set has many lines, while each line has one of the three kinds of action:
 * R, I, or D.
 *
 * So we have to use ActionType and one of the three payload object types to defind an
 * action.
 *
 * */
struct RPayload {
    const char* name;               // Relation name
    ValueType type;                 // Key type in the relation
    unsigned int size;              // Record size for each record including key
};

struct IPayload {
    const char* name;               // Relation name
    std::vector<Value>* values;     // Values, the first element is key
};

struct DPayload {
    const char* name;               // Relation name
    Value value;                    // key-value of which a record to be delete
};

union Payload {
    Payload() = delete;
    Payload(RPayload _r): r(_r) {}
    Payload(IPayload _i): i(_i) {}
    Payload(DPayload _d): d(_d) {}
    RPayload r;
    IPayload i;
    DPayload d;
};

struct Action {
    Action() = delete;
    Action(RPayload _r): payload(_r), type(ActionType::R) {}
    Action(IPayload _i): payload(_i), type(ActionType::I) {}
    Action(DPayload _d): payload(_d), type(ActionType::D) {}

    ActionType type;
    Payload payload;
};

/* Action manipulation functions*/
bool isValidRInput(const char*);
bool isValidIInput(const char*);
bool isValidDInput(const char*);
bool isValidInput(const char*);

#endif

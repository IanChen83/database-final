#ifndef ACTION_HEADER
#define ACTION_HEADER

#include <string>
#include <vector>
#include <utility>

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
 * U: Unknown action type, implementation defined
 * */
enum ActionType { U, R, I, D };


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
bool operator!= (const Value&, const Value&);



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

static inline void removeValue(Value*);

/*
 * A Record is a key(of type 'Value')/value(of type 'const char*')
 *
 * call createRecord(key, value) to create a new record in heap
 * call removeRecord(Record*) to remove a record
 *
 * */
typedef std::pair<Value*, const char*> Record;

Record* createRecord(int, const char*);
Record* createRecord(const char*, const char*);

static inline void removeRecord(Record*);
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
    std::vector<Record> values;     // Values are of Key/Record pairs
};

struct DPayload {
    const char* name;               // Relation name
    Value* value;                    // key-value of which a record to be delete
};

union Payload {
    Payload() = delete;
    ~Payload() {}
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

/*
 * A valid record should
 * 1. begin with "
 * 2. end with "
 * 3. have content (those between double quotes) more that one byte
 * 4. have content not containing " or ;
 *
 * A valid key should
 * 1. if it is of ValueType::Integer, it should be able to be parsed by atoi
 * 2. if it is of ValueType::String, it should start and end with " and not contain
 *    double quotes and ; in its content.
 *
 * */
bool isValidRecord(const char*);
bool isValidValue(const char*);

bool isValidRInput(const char*);
bool isValidIInput(const char*);
bool isValidDInput(const char*);
bool isValidInput(const char*);

RPayload getRPayload(const char*);
IPayload getIPayload(const char*);
DPayload getDPayload(const char*);
Action* getAction(const char*);
#endif

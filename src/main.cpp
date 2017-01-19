#include <iostream>
#include <fstream>
#include <cstring>
#include <map>
#include "bpt.h"
#include "action.h"
#include "value.h"
#include "rm.h"

using namespace std;

#define SIZE 1024

Value
getKeyFromValue(Value v) {
    if (v.type == ValueType::String) {
        char key[10];
        strncpy(key, v.StrValue, 10);
        return Value(key);
    } else {
        return v;
    }
}

class Relation {
    public:
        Relation(ValueType type, unsigned int length)
        : type(type), r_length(length)
        {
            if (type == ValueType::String) {
                tree = new bplus_tree(10, 42, 42, type);
            } else {
                tree = new bplus_tree(10, 85, 85, type);
            }
            rm = new RecordManager(type);
        }
        ~Relation() {
            delete tree;
            delete rm;
        }
        ValueType type;
        unsigned int r_length;
        bplus_tree* tree;
        RecordManager* rm;
};


int main(int argc, char* argv[]) {
    const char* filename = "../ProjectB_data.txt";
    char line[SIZE];
    fstream fin;
    fin.open(filename,ios::in);
    map<string, Relation*> relations;
    while(fin.getline(line,sizeof(line),'\n')){
        cout<<line<<endl;
        Action* action = getAction(line);
        if (action == NULL) continue;
        switch(action->type) {
            case ActionType::R :{
                string name = action->payload.r.name;
                relations[name] = new Relation(action->payload.r.type, action->payload.r.size);
                break;
            }
            case ActionType::I :{
                string name = action->payload.i.name;
                Relation* relation = relations[name];
                vector<Record> records = action->payload.i.values;
                for (int i=0, l=records.size(); i<l; i++) {
                    rid_t rid = relation->rm->addRecord(records[i]);
                    Value* v = get<0>(records[i]);
                    relation->tree->bplus_tree_insert(getKeyFromValue(*v), rid);
                }
                break;
            }
            case ActionType::D :{
                string name = action->payload.d.name;
                Relation* relation = relations[name];
                Value* v = action->payload.d.value;
                rid_t rid = relation->tree->bplus_tree_search(*v);
                if (rid < 0 || !relation->tree->bplus_tree_delete(*v)) {
                    printf("value doesn't exist\n");
                } else {
                    relation->rm->removeRecord(rid);
                }
                break;
            }
            case ActionType::S :{
                string name = action->payload.s.name;
                printf("# leaf page: %d\n #total index page: %d\n", relations[name]->tree->get_leaf_num(),relations[name]->tree->get_non_leaf_num());
                break;
            }
            case ActionType::Q :{
                string name = action->payload.q.name;
                Relation* r= relations[name];
                if (action->payload.q.value2 == NULL) {
                    // single query
                    Value* v = action->payload.q.value1;
                    rid_t rid = r->tree->bplus_tree_search(getKeyFromValue(*v));
                } else {
                    Value v1 = getKeyFromValue(*action->payload.q.value1);
                    Value v2 = getKeyFromValue(*action->payload.q.value2);
                    vector<rid_t> rid_list = r->tree->bplus_tree_get_range(v1, v2);
                }
                break;
            }
            case ActionType::P :{
                string name = action->payload.p.name;
                Relation* r= relations[name];
                uint16_t pid = action->payload.p.pid;
                for(int i = 0, l = r->rm->getPageLength(pid);i < l ; i++) {
                    Record record = r->rm->getRecord(to_rid(pid, i));
                }
                break;
            }
            case ActionType::C :
            break;
            default:
            break;
        }
        delete action;
    }
    return 0;
}

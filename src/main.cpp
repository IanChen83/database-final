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
    const char* outname = "../output.txt";
    char line[SIZE];
    fstream fin;
    fstream fout;
    fin.open(filename,ios::in);
    fout.open(outname, ios::out);
    if (!fin || !fout) {
        printf("open file error\n");
        return 1;
    }
    map<string, Relation*> relations;
    while(fin.getline(line,sizeof(line),'\n')){
        fout<<line<<endl;
        Action* action = getAction(line);
        if (action == NULL) {
            fout << "Invalid Input format!!\n";
            continue;
        }
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
                    Value* v = get<0>(records[i]);
                    if(v->type != relation->type) {
                        fout << "input type error\n";
                        continue;
                    }
                    rid_t rid = relation->rm->addRecord(records[i]);
                    fout << "#page: " << get_pid(rid) <<" #slot" << get_sn(rid) << endl;
                    relation->tree->bplus_tree_insert(getKeyFromValue(*v), rid);
                }
                break;
            }
            case ActionType::D :{
                string name = action->payload.d.name;
                Relation* relation = relations[name];
                Value* v = action->payload.d.value;
                if (v->type != relation->type) {
                    fout << "delete type error\n";
                    continue;
                }
                rid_t rid = relation->tree->bplus_tree_search(*v);
                if (rid < 0 || !relation->tree->bplus_tree_delete(*v)) {
                    fout << "value doesn't exist\n";
                } else {
                    relation->rm->removeRecord(rid);
                }
                break;
            }
            case ActionType::S :{
                string name = action->payload.s.name;
                int leaf_num = relations[name]->tree->get_leaf_num();
                int non_leaf_num = relations[name]->tree->get_non_leaf_num();
                fout << "# leaf page: " <<leaf_num<< "#total index page" << leaf_num+non_leaf_num << endl;
                break;
            }
            case ActionType::Q :{
                string name = action->payload.q.name;
                Relation* r= relations[name];
                if (action->payload.q.value2 == NULL) {
                    // single query
                    Value* v = action->payload.q.value1;
                    rid_t rid = r->tree->bplus_tree_search(getKeyFromValue(*v));
                    if (rid < 0) {
                        fout << "query value doesn't exist\n";
                        continue;
                    }

                    fout << "Key: " << getValueStr(*v)
                    << "size of rest of record: " << get<1>(r->rm->getRecord(rid)).length()
                    << "Rid: " << rid;
                } else {
                    Value v1 = getKeyFromValue(*action->payload.q.value1);
                    Value v2 = getKeyFromValue(*action->payload.q.value2);
                    vector<rid_t> rid_list = r->tree->bplus_tree_get_range(v1, v2);
                    fout << "Rid List: ";
                    for (auto& x: rid_list) {
                        fout << x << " ";
                    }
                    fout << endl;
                }
                break;
            }
            case ActionType::P :{
                string name = action->payload.p.name;
                Relation* r= relations[name];
                uint16_t pid = action->payload.p.pid;
                fout << "Content: ";
                for(int i = 0, l = r->rm->getPageLength(pid);i < l ; i++) {
                    Record record = r->rm->getRecord(to_rid(pid, i));
                    fout << getValueStr(*get<0>(record))<< " " <<get<1>(record) << ",";
                }
                fout<<endl;
                break;
            }
            case ActionType::C :{
                string name = action->payload.p.name;  
                Relation* relation= relations[name];
                int leaf_num = relation->tree->get_leaf_num();
                int non_leaf_num = relation->tree->get_non_leaf_num();
                int pageSize = relation->rm->pageSize();
                fout << "# index page: " << leaf_num+non_leaf_num << ", # slotted data pages " << pageSize << endl;
                break;
            }
            default:
            break;
        }
        delete action;
    }
    fin.close();
    return 0;
}

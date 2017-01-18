#include <iostream>
#include <fstream>
#include <map>
#include "bpt.h"
#include "action.h"
#include "rm.h"

using namespace std;

#define SIZE 1024

class Relation {
    public:
        Relation(ValueType type, unsigned int length)
        : type(type), r_length(length)
        {
            tree = new bplus_tree(10, 4, 4, type);
            rm = new RecordManager();
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
                break;
            }
            case ActionType::D :
            break;
            case ActionType::S :{
                string name = action->payload.s.name;
                printf("# leaf page: %d\n #total index page: %d\n", relations[name]->tree->get_leaf_num(),relations[name]->tree->get_non_leaf_num());
                break;
            }
            case ActionType::Q :{
                string name = action->payload.q.name;
                Relation* rel = relations[name];
                break;
            }
            case ActionType::P :
            break;
            case ActionType::C :
            break;
            default:
            break;
        }
        delete action;
    }
    return 0;
}

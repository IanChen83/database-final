#include <iostream>
#include <fstream>
#include <map>
#include "bpt.h"
#include "action.h"

using namespace std;

#define SIZE 1024


int main(int argc, char* argv[]) {
    char* filename = "../ProjectB_data.txt";
    char line[SIZE];
    fstream fin;
    fin.open(filename,ios::in);
    map<string, bplus_tree*> tree_manager;
    while(fin.getline(line,sizeof(line),'\n')){
        cout<<line<<endl;
        Action* action = getAction(line);
        if (action == NULL) continue;
        switch(action->type) {
            case ActionType::R :
                map[name] = new bplus_tree(10, 4, 4, action->paload->type);
            break;
            case ActionType::I :
            break;
            case ActionType::D :
            break;
            case ActionType::S :
            break;
            case ActionType::Q :
            break;
            case ActionType::P :
            break;
            case ActionType::C :
            break;
            default:
            break;
        }
    }
    return 0;
}

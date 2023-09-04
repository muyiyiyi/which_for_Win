#include <iostream>
#include <string>
#include <chrono>
#include "utils.h"

using namespace std;
using namespace std::chrono;

int main(int argc, char* argv[]) {
    if (argc !=2 &&  argc != 3){
        cout << "usage: \n which_for_win [option] query_value \n option:\n -fz  : fuzz search"<< endl;
        return 0;
    }
    bool match_fuzz = false; // 模糊查询 关键字存在即可

    for(int i=1;i <argc-1; i++){
        char* arg = argv[i];
        if (strcmp(arg, "-fz")==0) {match_fuzz= true;}
        else {cout << "input option must be -fz"; return 0;}
    }
    string query_cmd =argv[argc-1];
    auto start= system_clock::now();
    vector<string> results ;
    find_cmd_in_envpath(query_cmd, match_fuzz, results);
    duration<double> cost = system_clock::now() - start;
    cout << "find " << results.size() << " result\n";
    for(string& res:results){
        cout << res << endl;
    }
    cout << "find cost time "<< cost.count() << " s\n";
    return 0;
}
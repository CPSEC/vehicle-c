#ifndef _CAR_H
#define _CAR_H

#include <string>
#include <unordered_map>

#include "share.h"

using namespace std;

class Part;

class Car {
   public:
    State* state;
    unordered_map<string, pid_t> part_pid;
    Car();
    void Init();
    void CreateStateDir();
    void ForkPart();
    void Run();

    void SaveState(string part_name);
    void RestoreState(string part_name);
};

#endif
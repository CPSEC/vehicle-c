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
    unordered_map<PartType, pid_t> part_pid;
    Car();
    void Init();
    void InitState();
    void AddPart();
    void AddHeatbeat();
    void CreateStateDir();
    void ForkPart();
    void Run();

    void SaveState(PartType part);
    void RestoreState(PartType part);
};

#endif
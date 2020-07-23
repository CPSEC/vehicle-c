#ifndef _CAR_H
#define _CAR_H

#include <string>
#include <unordered_map>

#include "share.h"

using namespace std;

class Part;

class Car {
   public:
    State* state_;
    unordered_map<PartType, pid_t> part_pid_;
    Car();
    void Init();
    void InitState();
    void AddPart();
    void CreateStateDir();
    void ForkPart();
    void Run();

//    void SaveState(PartType part);
//    void RestoreState(PartType part);
//    void SimulateFalut(PartType part);
//    void CheckCompulsiveCheckpoint();
//    void CheckUnitCheckpoint();
};

#endif

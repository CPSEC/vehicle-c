#ifndef _SHARE_H
#define _SHARE_H

#include <ctime>
using namespace std;

#define SHARED_MEMORY_KEY 4444
#define HEARTBEAT_INTERVAL 5

#define PART_NUMBER 8
enum class PartType : int { camera, other };

class State {
   public:
    int speed_;
    int img_;
    time_t part_heartbeat[PART_NUMBER];
    // static State* shared_state_;
    static State* ShareMemoryInit();
    // static State* GetState();
    State(int speed);
    int GetSpeed();
    void SetSpeed(int speed);
};

#endif
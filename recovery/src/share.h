#ifndef _SHARE_H
#define _SHARE_H

#include <ctime>

#include "pca.h"

using namespace std;

#define MEM_NAME "shared_memory_name"
#define HEARTBEAT_INTERVAL 5

#define PART_NUMBER 8
enum class PartType : int { camera, servoPID, throttlePID, speed };

class State {
   public:
    double speed_;
    double direction_;
    double target_speed_;
    double target_direction_;
    Pca* pca_;
    time_t part_heartbeat[PART_NUMBER];
    static State* ShareMemoryInit();
};

#endif

#ifndef _SHARE_H
#define _SHARE_H

#include <sys/time.h>

#include <utility>

#include "pca.h"
using namespace std;

#define MEM_NAME "shared_memory_name"
#define HEARTBEAT_INTERVAL 5

#define PART_NUMBER 4
enum class PartType : int { camera, servoPID, throttlePID, speed };

class State {
   public:
    double speed_;
    double direction_;
    double target_speed_;
    double target_direction_;

    // indicate if the 4 variable above is new data or not
    bool is_new_data[PART_NUMBER];

    // indicate if the corresponding part need a compulsive checkpoint
    bool need_compulsive_checkpoint[PART_NUMBER];

    // indicate average one cycle time for each part
    suseconds_t average_time_with_cycles[PART_NUMBER];

    // indicate the last unit(time based) checkpoint time
    timeval last_unit_checkpoint[PART_NUMBER];

    Pca* pca_;
    static State* ShareMemoryInit();
};

#endif

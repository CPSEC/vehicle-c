#ifndef _SHARE_H
#define _SHARE_H

#include <sys/time.h>

#include <iostream>

using namespace std;

#define MEM_NAME "shared_memory_name"
#define PART_NUMBER 4
#define DATA_NUMBER 4

// for debug
#define DBG (cout << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << endl)
//

enum class PartType : int { camera, servoPID, throttlePID, speed };

class State {
   public:
    // for test
    int times;

    double speed_;
    double direction_;
    double target_speed_;
    double target_direction_;

    // indicate if the 4 variable above is new data or not
    bool is_new_data[DATA_NUMBER];

    // indicate if the corresponding part need a compulsive checkpoint
    bool need_compulsive_checkpoint[PART_NUMBER];

    // indicate average one cycle time for each part
    suseconds_t average_time_per_cycle[PART_NUMBER];

    // indicate the last unit(time based) checkpoint time
    timeval last_unit_checkpoint[PART_NUMBER];

    static State* ShareMemoryInit();
    static State* ShareMemoryInitCar();
};

#endif

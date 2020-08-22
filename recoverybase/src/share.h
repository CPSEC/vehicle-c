#ifndef _SHARE_H
#define _SHARE_H

#include <sys/time.h>

#include <iostream>

using namespace std;

#define MEM_NAME "shared_memory_name"
#define PART_NUMBER 3  // 4
#define DATA_NUMBER 3  // 4

// for debug

// 1 only unit, 2 only compulsive, 3 both
#define METHOD 2

// true without falut, false with one falut
#define FAULTED true
#define INTERVAL 2

#define MAX_TIMES 30
#define FAULT_STEP 5

#define avgt0 0
#define avgt1 0
#define avgt2 0
// #define avgt3 0

#define DBG ;
//(cout << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << endl)
//

enum class PartType : int { camera, speed, servoPID, throttlePID };

suseconds_t diffus(timeval& tv_start, timeval& tv_end);
void costtime(int n = 2048);

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

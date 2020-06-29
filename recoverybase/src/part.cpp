#include "part.h"

#include <sys/types.h>
#include <unistd.h>

#include <cstdlib>

Part::Part(PartType parttype) {
    part_type_ = parttype;
    state_ = State::ShareMemoryInit();
}

// set this part need a compulsive checkpoint
void Part::SetCompulsiveCheckpoint() {
    state_->need_compulsive_checkpoint[(int)part_type_] = true;
}

// wait the manager make a checkpoint and set need compulsive to false
void Part::WaitCompulsiveCheckpoint() {
    // while (state_->need_compulsive_checkpoint[(int)part_type_]) {
    //     sleep(1);
    // }
}

void Part::UpdateAverageCycleTime(timeval& tv_start, timeval& tv_end) {
    suseconds_t t = diffus(tv_start, tv_end);
    // cout << "!!!!!!!!!!!!!!!!!!!!!!! check ";
    // for (int i = 0; i < PART_NUMBER; ++i)
    //     cout << state_->average_time_per_cycle[i] << " ";
    // cout << endl;
    state_->average_time_per_cycle[(int)part_type_] =
        (state_->average_time_per_cycle[(int)part_type_] + t) / 2;
    // cout << "!!!!!!!!!!!!!!!!!!!!!!! update "
    //      << " new " << state_->average_time_per_cycle[(int)part_type_] <<
    //      endl;
}

// wait until needed data is new data
// double speed_;
// double direction_;
// double target_speed_;
// double target_direction_;
void Part::WaitNeededNewData() {
    // while (state_->is_new_data[(int)part_type_] == false) {
    //     usleep(50);
    // }
    state_->is_new_data[(int)part_type_] = false;
}

// update output data to new data
void Part::UpdateIsNewData() {}

// Run for one cycle
// void Part::Run() = 0;

// a loop for one all things need to do
void Part::RunALL() {
    DBG;
    while (true) {
        timeval tv_start, tv_end;
        gettimeofday(&tv_start, nullptr);
        WaitNeededNewData();

        Run();
        UpdateIsNewData();
        gettimeofday(&tv_end, nullptr);
        UpdateAverageCycleTime(tv_start, tv_end);

        SetCompulsiveCheckpoint();
        WaitCompulsiveCheckpoint();
        if (state_->times >= MAX_TIMES) exit(0);
    }
}

Part::~Part() {}
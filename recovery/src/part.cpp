#include "part.h"

#include <unistd.h>

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
    while (state_->need_compulsive_checkpoint[(int)part_type_]) {
        usleep(50);
    }
}

void Part::UpdateAverageCycleTime(timeval& tv_start, timeval& tv_end) {
    suseconds_t t = tv_end.tv_usec - tv_start.tv_usec;
    if (state_->average_time_with_cycles[(int)part_type_] == 0) {
        state_->average_time_with_cycles[(int)part_type_] = t;
    } else {
        state_->average_time_with_cycles[(int)part_type_] =
            (state_->average_time_with_cycles[(int)part_type_] + t) / 2;
    }
}
// Run for one cycle
void Part::Run(){};

// a loop for one all things need to do
void Part::RunALL() {
    while (true) {
        while (state_->is_new_data[(int)part_type_] == false) {
            usleep(50);
        }
        state_->is_new_data[(int)part_type_] = false;
        timeval tv_start, tv_end;
        gettimeofday(&tv_start, nullptr);
        Run();
        UpdateIsNewData();
        gettimeofday(&tv_end, nullptr);
        UpdateAverageCycleTime(tv_start, tv_end);
        SetCompulsiveCheckpoint();
        WaitCompulsiveCheckpoint();
    }
}

Part::~Part() {}
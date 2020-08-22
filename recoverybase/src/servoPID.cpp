#include "servoPID.h"

#include "unistd.h"

using namespace std;

ServoPID::ServoPID() : Part(PartType::servoPID) {}

void ServoPID::WaitNeededNewData() {
    // // target_direction_
    // while (state_->is_new_data[1] == false && state_->is_new_data[3] ==
    // false) {
    //     usleep(50);
    // }
    // state_->is_new_data[1] = false;
    // state_->is_new_data[3] = false;
    // return;
    while (state_->is_new_data[1] == false) {
        // usleep(10);
    }
    state_->is_new_data[1] = false;
    return;
}

void ServoPID::UpdateIsNewData() {
    state_->is_new_data[2] = true;
    return;
}

void ServoPID::Run() {
    DBG;
    costtime();
    // timeval t1, t2;
    // gettimeofday(&t1, nullptr);
    // gettimeofday(&t2, nullptr);
    // while (diffus(t1, t2) < avgt1) {
    //     usleep(100);
    //     gettimeofday(&t2, nullptr);
    // }
    state_->times += 1;
    return;
}

ServoPID::~ServoPID() {}
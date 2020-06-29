#include "throttlePID.h"

#include <unistd.h>

ThrottlePID::ThrottlePID() : Part(PartType::throttlePID) {}

void ThrottlePID::WaitNeededNewData() {
    // target_speed_
    while (state_->is_new_data[0] == false && state_->is_new_data[2] == false) {
        usleep(50);
    }
    state_->is_new_data[0] = false;
    state_->is_new_data[2] = false;
    return;
}

void ThrottlePID::UpdateIsNewData() { return; }

void ThrottlePID::Run() {
    DBG;
    // costtime(26);
    timeval t1, t2;
    gettimeofday(&t1, nullptr);
    gettimeofday(&t2, nullptr);
    while (diffus(t1, t2) < avgt2) {
        usleep(100);
        gettimeofday(&t2, nullptr);
    }
}

ThrottlePID::~ThrottlePID() {}
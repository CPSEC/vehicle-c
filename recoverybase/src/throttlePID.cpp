#include "throttlePID.h"

#include <unistd.h>

ThrottlePID::ThrottlePID() : Part(PartType::throttlePID) {}

void ThrottlePID::WaitNeededNewData() {
    // target_speed_
    while (state_->is_new_data[2] == false) {
        usleep(50);
    }
    state_->is_new_data[2] = false;
    return;
}

void ThrottlePID::UpdateIsNewData() { return; }

void ThrottlePID::Run() {
    DBG;
    sleep(1);
}

ThrottlePID::~ThrottlePID() {}
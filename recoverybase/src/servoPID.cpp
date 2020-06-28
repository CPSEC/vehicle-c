#include "servoPID.h"

#include "unistd.h"

using namespace std;

ServoPID::ServoPID() : Part(PartType::servoPID) {}

void ServoPID::WaitNeededNewData() {
    // target_direction_
    while (state_->is_new_data[3] == false) {
        usleep(50);
    }
    state_->is_new_data[3] = false;
    return;
}

void ServoPID::UpdateIsNewData() { return; }

void ServoPID::Run() {
    DBG;
    sleep(1);
}

ServoPID::~ServoPID() {}
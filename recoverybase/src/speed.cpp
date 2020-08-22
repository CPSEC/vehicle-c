#include "speed.h"

#include <unistd.h>

using namespace std;

Speed::Speed() : Part(PartType::speed) {}

void Speed::WaitNeededNewData() {
    while (state_->is_new_data[0] == false) {
        // usleep(10);
    }
    state_->is_new_data[0] = false;
    return;
}

void Speed::UpdateIsNewData() {
    // state_->is_new_data[0] = true;
    // state_->is_new_data[1] = true;
    state_->is_new_data[1] = true;
    return;
}

void Speed::Run() {
    DBG;
    costtime();
    // timeval t1, t2;
    // gettimeofday(&t1, nullptr);
    // gettimeofday(&t2, nullptr);
    // while (diffus(t1, t2) < avgt3) {
    //     usleep(100);
    //     gettimeofday(&t2, nullptr);
    // }
    // cout << "speed " << diffus(t1, t2) << endl;
}

Speed::~Speed() {}

#include "speed.h"

#include <unistd.h>

using namespace std;

Speed::Speed() : Part(PartType::speed) {}

void Speed::WaitNeededNewData() { return; }

void Speed::UpdateIsNewData() {
    state_->is_new_data[0] = true;
    state_->is_new_data[1] = true;
    return;
}

void Speed::Run() {
    DBG;
    // usleep(avgt3);
    // int i = 40;
    // while (i--) costtime(26);
    timeval t1, t2;
    gettimeofday(&t1, nullptr);
    gettimeofday(&t2, nullptr);
    while (diffus(t1, t2) < avgt3) {
        usleep(100);
        gettimeofday(&t2, nullptr);
    }
    // cout << "speed " << diffus(t1, t2) << endl;
}

Speed::~Speed() {}

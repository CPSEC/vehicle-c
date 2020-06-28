#include "speed.h"

#include <unistd.h>

using namespace std;

Speed::Speed() : Part(PartType::speed) {}

void Speed::WaitNeededNewData() { return; }

void Speed::UpdateIsNewData() {
    state_->is_new_data[0] = true;
    return;
}

void Speed::Run() {
    DBG;
    sleep(2);
}

Speed::~Speed() {}

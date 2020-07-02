#include "throttlePID.h"

#include <unistd.h>

#include <iostream>

#include "pca.h"
using namespace std;

ThrottlePID::ThrottlePID()
    : Part(PartType::throttlePID),
      pid(MiniPID(0.00228677780020377, 0.00277411402751383,
                  -0.000169192834187225)) {
    pid.setOutputLimits(0.0, 1.0);
}

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
    double output = pid.getOutput(state_->speed_, state_->target_speed_);
    double impulse = output / 2 + 1.5;
    state_->pca_->setSpeed(impulse);
}

ThrottlePID::~ThrottlePID() {}

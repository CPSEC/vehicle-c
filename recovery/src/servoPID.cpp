#include "servoPID.h"

#include <iostream>
#include <unistd.h>

#include "pca.h"

using namespace std;

ServoPID::ServoPID()
    : Part(PartType::servoPID),
      pid(MiniPID(0.00228677780020377, 0.00277411402751383,
                  -0.000169192834187225)) {
    pid.setOutputLimits(-1.0, 1.0);
}

void ServoPID::WaitNeededNewData() {
    // target_direction_
    while (state_->is_new_data[1] == false) {
        usleep(50);
    }
    if(state_->is_new_data[3]){
        state_->is_new_data[1] = false;
    }
    return;
}
void ServoPID::UpdateIsNewData() {
    return;
}

void ServoPID::Run() {
    if(state_->is_new_data[4]){
        pid.setPID(state_->servoPID_[0],state_->servoPID_[1],state_->servoPID_[2]);
        state_->is_new_data[4] = false;
    }
    double output =
        pid.getOutput(state_->direction_, 0);
    state_->pca_->setDirect(output / 2 + 1.5);
    state_->direction_ = output / 2 + 1.5;
}

ServoPID::~ServoPID() {}

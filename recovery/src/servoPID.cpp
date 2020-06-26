#include "servoPID.h"

#include <iostream>
using namespace std;
#include "MiniPID.h"

ServoPID::ServoPID() : Part(PartType::servoPID) {
    MiniPID pid = MiniPID(0.00228677780020377, 0.00277411402751383,
                          -0.000169192834187225);
    pid.setOutputLimits(-1.0, 1.0);
}

void ServoPID::Run() {
    while (true) {
        // cout << "dir: " << car_->direction <<"target: " <<
        // car_->target_direction << endl;
        double output =
            pid.getOutput(state_->direction_, state_->target_direction_);
        // apply the target impulse duration;
        // cout << "ServoPID: " << output/2 + 1.5 <<endl;
        state_->pca_->setDirect(output / 2 + 1.5);
        state_->direction_ = output / 2 + 1.5;
    }
}

ServoPID::~ServoPID() {}
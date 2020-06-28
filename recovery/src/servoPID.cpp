#include "servoPID.h"

#include <iostream>

#include "pca.h"

using namespace std;

ServoPID::ServoPID()
    : Part(PartType::servoPID),
      pid(MiniPID(0.00228677780020377, 0.00277411402751383,
                  -0.000169192834187225)) {
    pid.setOutputLimits(-1.0, 1.0);
}

void ServoPID::Run() {
    double output =
        pid.getOutput(state_->direction_, state_->target_direction_);
    state_->pca_->setDirect(output / 2 + 1.5);
    state_->direction_ = output / 2 + 1.5;
}

ServoPID::~ServoPID() {}
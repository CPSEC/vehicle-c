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

void ThrottlePID::Run() {
    double output = pid.getOutput(state_->speed_, state_->target_speed_);
    double impulse = output / 2 + 1.5;
    state_->pca_->setSpeed(impulse);
    state_->speed_ = impulse;
}

ThrottlePID::~ThrottlePID() {}
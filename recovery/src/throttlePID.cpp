#include "throttlePID.h"

#include <unistd.h>

#include <iostream>

#include "MiniPID.h"
using namespace std;

ThrottlePID::ThrottlePID() : Part(PartType::throttlePID) {
    MiniPID pid = MiniPID(0.00228677780020377, 0.00277411402751383,
                          -0.000169192834187225);
    pid.setOutputLimits(0.0, 1.0);
}

void ThrottlePID::Run() {
    // cout << "###########################" << endl;
    usleep(1000000);
    // cout << "*************************" << endl;
    while (true) {
        // cout << "speed1: " << car_->speed << endl;
        double output = pid.getOutput(state_->speed_, state_->target_speed_);
        // apply the target impulse duration;
        double impulse = output / 2 + 1.5;
        // cout << "speed2: " << car_->speed << endl;
        // cout << "##############output1: " << impulse << endl;
        // if(impulse <= 1.5){impulse = 1.5;}
        // if(impulse >= 1.77){impulse = 1.77;}
        // cout << "##############***********output2: " << impulse << endl;
        state_->pca_->setSpeed(impulse);
        state_->speed_ = impulse;
        usleep(60000);
    }
}

ThrottlePID::~ThrottlePID() {}
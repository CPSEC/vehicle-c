#include "throttlePID.h"
#include <iostream>
#include "MiniPID.h"
#include <unistd.h>
using namespace std;

ThrottlePID::ThrottlePID(Car* car){
    name_ ="ThrottlePID";
    car_ = car;
    is_thread_ = true;
    pid.setOutputLimits(0.0, 1.0);
}

void ThrottlePID::Run(){
    //cout << "###########################" << endl;
    usleep(1000000);
    //cout << "*************************" << endl;
    while(true){
        //cout << "speed1: " << car_->speed << endl;
        double output = pid.getOutput(car_->speed, car_->target_speed);
        //apply the target impulse duration;
        double impulse = output/2 + 1.5;
        //cout << "speed2: " << car_->speed << endl;
        //cout << "##############output1: " << impulse << endl;
        //if(impulse <= 1.5){impulse = 1.5;}
        //if(impulse >= 1.77){impulse = 1.77;}
        //cout << "##############***********output2: " << impulse << endl;
        car_->seq_parts_["Pca9685"]->setSpeed(impulse);
        usleep(60000);
    }
}
void ThrottlePID::StartSeq(){
    
}

void ThrottlePID::StartThread(){
    cout << name_ <<endl;
    thread_ = new thread(&ThrottlePID::Run, this);
    thread_ -> detach();
}

ThrottlePID::~ThrottlePID(){
    
}



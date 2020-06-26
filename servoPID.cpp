#include "servoPID.h"
#include <iostream>
using namespace std;
#include "MiniPID.h"

ServoPID::ServoPID(Car* car){
    name_ ="ServoPID";
    car_ = car;
    is_thread_ = true;
    pid.setOutputLimits(-1.0, 1.0);
}

void ServoPID::Run(){
    while(true){
        //cout << "dir: " << car_->direction <<"target: " << car_->target_direction << endl;
        double output = pid.getOutput(car_->direction, car_->target_direction);
        //apply the target impulse duration;
        //cout << "ServoPID: " << output/2 + 1.5 <<endl;
        car_->seq_parts_["Pca9685"]->setDirect(output/2 + 1.5);
        car_->direction = output/2 + 1.5;
    }
}
void ServoPID::StartSeq(){
    
}

void ServoPID::StartThread(){
    cout << name_ <<endl;
    thread_ = new thread(&ServoPID::Run, this);
    thread_ -> detach();
}

ServoPID::~ServoPID(){
    
}


#include "part.h"
#include "car.h"

Part::Part(Car *car,bool is_thread) {
    //
    name_ = "";
    is_thread_ = is_thread;
    thread_ = nullptr;
    car_=car;
}

Part::~Part() {
    //
}

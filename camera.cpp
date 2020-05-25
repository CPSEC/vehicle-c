#include "camera.h"
#include "car.h"
#include <iostream>

Camera::Camera(Car*car,bool is_thread) : Part(car,is_thread) {
    //
    name_ = "Camera thread";
}

void Camera::Run(int test) {
    //
    int times = 1;
    chrono::system_clock::time_point start;
    chrono::system_clock::time_point end;

    while (times) {
        start = chrono::system_clock::now();
        cout << test << endl;
        car_->test+=1;
        this_thread::sleep_for(chrono::seconds(1));
        end = chrono::system_clock::now();
        cout << chrono::duration_cast<chrono::milliseconds>(end - start).count()
             << endl;
    }
}

void Camera::StartSeq() {
    //
}

void Camera::StartThread() {
    //
    cout << name_ << endl;
    thread_ = new thread(&Camera::Run, this, 123);
    thread_->detach();
}

Camera::~Camera() {
    //
}
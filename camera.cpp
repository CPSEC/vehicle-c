#include "camera.h"

#include <iostream>

Camera::Camera(bool is_thread) : Part(is_thread) {
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
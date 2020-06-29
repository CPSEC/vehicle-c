#include "camera.h"

#include <unistd.h>
using namespace std;

Camera::Camera() : Part(PartType::camera) {}

void Camera::UpdateIsNewData() {
    // target_speed_
    state_->is_new_data[2] = true;
    // target_directiont_
    state_->is_new_data[3] = true;
}

void Camera::WaitNeededNewData() { return; }

void Camera::Run() {
    DBG;
    // int i = 80;
    // while (i--) costtime(26);
    // int cnt = state_->times;
    timeval t1, t2;
    gettimeofday(&t1, nullptr);
    gettimeofday(&t2, nullptr);
    while (diffus(t1, t2) < avgt0) {
        usleep(100);
        gettimeofday(&t2, nullptr);
    }
    state_->times += 1;
    // state_->times = 1 + state_->times > cnt ? state_->times : cnt;
    cout << "Camera add times to " << state_->times << endl;
}

Camera::~Camera() {}
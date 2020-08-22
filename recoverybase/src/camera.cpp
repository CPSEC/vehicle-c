#include "camera.h"

#include <unistd.h>
using namespace std;

Camera::Camera() : Part(PartType::camera) {}

void Camera::WaitNeededNewData() { return; }

void Camera::UpdateIsNewData() {
    // // target_speed_
    // state_->is_new_data[2] = true;
    // // target_directiont_
    // state_->is_new_data[3] = true;

    state_->is_new_data[0] = true;
    return;
}

void Camera::Run() {
    DBG;
    // timeval t1, t2;
    // gettimeofday(&t1, nullptr);
    // gettimeofday(&t2, nullptr);
    // while (diffus(t1, t2) < avgt0) {
    //     usleep(100);
    //     gettimeofday(&t2, nullptr);
    // }
    costtime();
    // state_->times += 1;
    // state_->times = 1 + state_->times > cnt ? state_->times : cnt;
    // cout << "Camera add times to " << state_->times << endl;
    return;
}

Camera::~Camera() {}
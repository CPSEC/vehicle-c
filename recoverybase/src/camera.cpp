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
    sleep(3);
    state_->times += 1;
    cout << "Camera add times to " << state_->times << endl;
}

Camera::~Camera() {}
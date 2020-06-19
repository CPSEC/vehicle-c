#include "camera.h"

#include <unistd.h>

#include <iostream>

#include "share.h"

using namespace std;

Camera::Camera() {
    state = State::ShareMemoryInit();
    part_type_ = PartType::camera;
}

void Camera::Run() {
    while (true) {
        state->img_ += 1;
        cout << "camera" << state->img_ << endl;
        state->part_heartbeat[(int)part_type_] = time(NULL);
        sleep(1);
    }
}
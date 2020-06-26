#include "car.h"

#include "part.h"

// example
#include "camera.h"
#include "speed.h"
#include "pca.h"
#include "throttlePID.h"
#include "servoPID.h"

Car::Car() {
    //
    test=0;
    speed = 0.0;
    direction = 1.5;
    target_speed = 50;
    target_direction = 1.5;
    Init();
    StartAll();
}

void Car::Init() {
    // add parts
    AddPart(new Camera(this,true));
    AddPart(new Speed(this));
    AddPart(new Pca(this));
    AddPart(new ThrottlePID(this));
    AddPart(new ServoPID(this));
}

void Car::AddPart(Part *p) {
    //
    if (p->is_thread_) {
        //
        thread_parts_[p->name_] = p;
    } else {
        seq_parts_[p->name_] = p;
    }
}

void Car::StartSequence() {
    // start parts need to be executed sequentially
    while (true) {
        cout << "speed:"<< speed << endl;
        for (pair<const string, Part *> &sp : seq_parts_) {
            sp.second->StartSeq();
        }
        this_thread::sleep_for(chrono::seconds(1));
    }
}

void Car::StartThread() {
    // start parts need to be executed in thread
    for (pair<const string, Part *> &sp : thread_parts_) {
        sp.second->StartThread();
    }
}

void Car::StartAll() {
    //
    StartThread();
    StartSequence();
}

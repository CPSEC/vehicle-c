#include "share.h"

#include <sys/shm.h>

#include <iostream>

// State* State::shared_state_ = ShareMemoryInit();

State* State::ShareMemoryInit() {
    int shmid =
        shmget((key_t)SHARED_MEMORY_KEY, sizeof(State), 0666 | IPC_CREAT);
    if (shmid == -1) {
        cerr << "shmget failed" << endl;
        return nullptr;
    }
    void* shm = shmat(shmid, 0, 0);
    if (shm == (void*)-1) {
        cerr << "shmat failed" << endl;
        return nullptr;
    }
    State* state = (State*)shm;
    // state->speed_ = 0;
    // state->speed_ = 0;
    return state;
}

// State* State::GetState() { return State::shared_state_; }

int State::GetSpeed() { return this->speed_; }

void State::SetSpeed(int speed) { this->speed_ = speed; }

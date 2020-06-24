#include "share.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>

// State* State::shared_state_ = ShareMemoryInit();

// State* State::ShareMemoryInit() {
//     int shmid =
//         shmget((key_t)SHARED_MEMORY_KEY, sizeof(State), 0666 | IPC_CREAT);
//     if (shmid == -1) {
//         cerr << "shmget failed" << endl;
//         return nullptr;
//     }
//     void* shm = shmat(shmid, 0, 0);
//     if (shm == (void*)-1) {
//         cerr << "shmat failed" << endl;
//         return nullptr;
//     }
//     State* state = (State*)shm;
//     // state->speed_ = 0;
//     // state->speed_ = 0;
//     return state;
// }

State* State::ShareMemoryInit() {
    int fd = shm_open(MEM_NAME, O_CREAT | O_TRUNC | O_RDWR, 0666);
    if (fd < 0) cerr << "shm_open failed" << endl;
    ftruncate(fd, sizeof(State));
    State* ptr = (State*)mmap(nullptr, sizeof(State), PROT_READ | PROT_WRITE,
                              MAP_SHARED, fd, 0);
    return ptr;
}

// State* State::GetState() { return State::shared_state_; }

int State::GetSpeed() { return this->speed_; }

void State::SetSpeed(int speed) { this->speed_ = speed; }

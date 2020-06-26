#include "share.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>

State* State::ShareMemoryInit() {
    int fd = shm_open(MEM_NAME, O_CREAT | O_TRUNC | O_RDWR, 0666);
    if (fd < 0) cerr << "shm_open failed" << endl;
    ftruncate(fd, sizeof(State));
    State* ptr = (State*)mmap(nullptr, sizeof(State), PROT_READ | PROT_WRITE,
                              MAP_SHARED, fd, 0);
    return ptr;
}
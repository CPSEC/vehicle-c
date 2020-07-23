#include "share.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>

suseconds_t diffus(timeval& tv_start, timeval& tv_end) {
    suseconds_t t1 = 1000000 * tv_start.tv_sec + tv_start.tv_usec;
    suseconds_t t2 = 1000000 * tv_end.tv_sec + tv_end.tv_usec;
    return t2 > t1 ? t2 - t1 : t1 - t2;
}

// 26 = 825
// 30 = 6374
// 35 = 66565
// 37 = 270521
int costtime(int n) {
    if (n <= 1)
        return n;
    else
        return costtime(n - 1) + costtime(n + 2);
}

State* State::ShareMemoryInit() {
    DBG;
    //int fd = shm_open(MEM_NAME, O_RDWR, 0666);
    int fd = shm_open(MEM_NAME, O_CREAT | O_TRUNC | O_RDWR, 0666);
    if (fd < 0) cerr << "shm_open failed" << endl;
    struct stat s;
    fstat(fd, &s);
    State* ptr = (State*)mmap(nullptr, s.st_size, PROT_READ | PROT_WRITE,
                              MAP_SHARED, fd, 0);
    close(fd);
    return ptr;
}
State* State::ShareMemoryInitCar() {
    DBG;
    shm_unlink(MEM_NAME);
    int fd = shm_open(MEM_NAME, O_CREAT | O_TRUNC | O_RDWR, 0666);
    if (fd < 0) cerr << "shm_open failed" << endl;
    // without -> bus error
    ftruncate(fd, sizeof(State));
    //
    State* ptr = (State*)mmap(nullptr, sizeof(State), PROT_READ | PROT_WRITE,
                              MAP_SHARED, fd, 0);
    close(fd);
    return ptr;
}

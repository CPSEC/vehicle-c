#include "car.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <iostream>

#include "camera.h"
//#include "criu.h"
#include "part.h"
#include "pca.h"
#include "servoPID.h"
#include "speed.h"
#include "throttlePID.h"
#include "remote.h"

using namespace std;

Car::Car() {
    DBG;
    Init();
}

void Car::Init() {
    DBG;
    InitState();
    AddPart();
    CreateStateDir();
    ForkPart();
    cout << "init finished" << endl;
}

// init shared memory -- state
void Car::InitState() {
    DBG;
    state_ = State::ShareMemoryInit();
    state_->speed_ = 0;
    state_->direction_ = 0;
    state_->target_speed_ = 0;
    state_->pca_ = new Pca();
    timeval tv;
    gettimeofday(&tv, nullptr);
    for (int i = 0; i < PART_NUMBER; ++i) {
        state_->is_new_data[i] = false;
        state_->need_compulsive_checkpoint[i] = false;
        state_->average_time_per_cycle[i] = 0;
        state_->last_unit_checkpoint[i] = tv;
    }
}

// add all parts
void Car::AddPart() {
    DBG;
    part_pid_[PartType::camera] = -1;
    part_pid_[PartType::servoPID] = -1;
    part_pid_[PartType::throttlePID] = -1;
    part_pid_[PartType::speed] = -1;
    part_pid_[PartType::remote] = -1;
}

// create part process
void Car::ForkPart() {
    DBG;
    for (pair<PartType, time_t> pr : part_pid_) {
        pid_t pid = fork();
        if (pid < 0) {
            cerr << "part fork err" << endl;
        } else if (pid == 0) {
            // part
            Part *part = nullptr;
            switch (pr.first) {
                case PartType::camera:
                    part = new Camera();
                    break;
                case PartType::servoPID:
                    part = new ServoPID();
                    break;
                case PartType::throttlePID:
                    part = new ThrottlePID();
                    break;
                case PartType::speed:
                    part = new Speed();
                    break;
                case PartType::remote:
                    part = new Remote();
                    break;
                default:
                    break;
            }
            if (part == nullptr) {
                cerr << "fork part err" << endl;
            } else {
                part->RunALL();
                cout << "start part" << (int)pr.first << endl;
            }
        } else {
            // car
            part_pid_[pr.first] = pid;
            cout << pid << endl;
            // waitpid(pid, nullptr, WNOHANG);
        }
    }
}

// run the car manager run loop
// moniter checkpoint
void Car::Run() {
    DBG;
    while (true) {
        for (pair<PartType, pid_t> pr : part_pid_) {
            //SaveState(pr.first);
        }
        sleep(1);
    }
}

// create folder to save process state
void Car::CreateStateDir() {
    DBG;
    mkdir("checkpoint", 0777);
    for (pair<PartType, pid_t> pr : part_pid_) {
        string dir = "./checkpoint/" + to_string((int)pr.first);
        if (mkdir(dir.c_str(), 0777) == -1) {
            cerr << "mkdir failed " << (int)pr.first << endl;
        }
    }
}

// dump a part process state according to part type
//void Car::SaveState(PartType part) {
//    DBG;
//    string dir = "./checkpoint/" + to_string((int)part);
//    if (criu_init_opts() == -1) cout << "criu init failed" << endl;
//    criu_set_pid(part_pid_[part]);
//    int fd = open((char *)dir.c_str(), O_DIRECTORY);
//    criu_set_images_dir_fd(fd);
//    criu_set_leave_running(true);
//    criu_set_shell_job(true);
//    criu_set_log_file("dump.log");
//    criu_set_log_level(4);
//    criu_dump();
//}

// restore a part process state according to part type
//void Car::RestoreState(PartType part) {
//    DBG;
//    string dir = "./checkpoint/" + to_string((int)part);
//    if (criu_init_opts() == -1) cout << "criu init failed" << endl;
//    int fd = open((char *)dir.c_str(), O_DIRECTORY);
//    criu_set_images_dir_fd(fd);
//    criu_set_shell_job(true);
//    criu_set_log_file("resotre.log");
//    criu_set_log_level(4);
//    criu_restore();
//}

// simulate a fault for a part process according to part type
//void Car::SimulateFalut(PartType part) {
//    // kill simulate attack
//    cout << "kill " << (int)part << endl;
//    char pid_buf[16];
//    sprintf(pid_buf, "%d", (int)part_pid_[part]);
//    char *cmd[] = {"sudo", "kill", "-s", "2", pid_buf, NULL};
//    pid_t p = fork();
//    if (p < 0) {
//        cerr << "kill failed" << endl;
//    } else if (p == 0) {
//        // child
//        execvp("sudo", cmd);
//    } else {
//        // parent
//        wait(0);
//    }
//}

// check if any part need a compulsive checkpoint
//void Car::CheckCompulsiveCheckpoint() {
//    for (int i = 0; i < PART_NUMBER; ++i) {
//        if (state_->need_compulsive_checkpoint[i]) {
//            SaveState((PartType(i)));
//            state_->need_compulsive_checkpoint[i] = false;
//        }
//    }
//}

// check if any part need a unit checkpoint
//void Car::CheckUnitCheckpoint() {
//    timeval tv;
//    gettimeofday(&tv, nullptr);
//    for (int i = 0; i < PART_NUMBER; ++i) {
//        if (tv.tv_usec - state_->last_unit_checkpoint[i].tv_usec >
//            state_->average_time_per_cycle[i] / 2) {
//            SaveState((PartType(i)));
//            state_->last_unit_checkpoint[i] = tv;
//        }
//    }
//}

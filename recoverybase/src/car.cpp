#include "car.h"

#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <iostream>

#include "camera.h"
#include "criu.h"
#include "part.h"
#include "servoPID.h"
#include "speed.h"
#include "throttlePID.h"

using namespace std;

Car::Car() {
    DBG;
    Init();
}

void Car::Init() {
    DBG;
    InitState();

    AddPart();
    ForkPart();
    CreateStateDir();

    cout << "car " << getpid() << endl;
    for (int i = 0; i < PART_NUMBER; ++i)
        cout << "part " << part_pid_[PartType(i)] << endl;

    cout << "init finished" << endl;
}

// init shared memory -- state
void Car::InitState() {
    DBG;
    state_ = State::ShareMemoryInitCar();
    // up to 10 times
    state_->times = 0;
    state_->speed_ = 0;
    state_->direction_ = 0;
    state_->target_speed_ = 0;
    state_->target_direction_ = 0;
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
}

// create part process
void Car::ForkPart() {
    cout << __FUNCTION__ << endl;
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
        }
    }
}

// run the car manager run loop
// moniter checkpoint
void Car::Run() {
    DBG;
    timeval t_start, t_end;
    gettimeofday(&t_start, nullptr);
    int last_time = state_->times;
    while (true) {
        cout << "\n\n\n=============================== " << state_->times
             << " ===============================" << endl;
        CheckCompulsiveCheckpoint();
        CheckUnitCheckpoint();
        if (state_->times >= 10) return;
        if (last_time != state_->times && state_->times % 2 == 0) {
            SimulateFalut(PartType::camera);
            RestoreState(PartType::camera);
            last_time = state_->times;
        }
        sleep(1);
    }
    gettimeofday(&t_end, nullptr);
    cout << "total run time " << t_end.tv_usec - t_start.tv_usec << " ms"
         << endl;
}

// create folder to save process state
void Car::CreateStateDir() {
    DBG;
    mkdir("checkpoint", 0777);
    for (pair<PartType, pid_t> p : part_pid_) {
        string dir = "./checkpoint/" + to_string(p.second);
        if (mkdir(dir.c_str(), 0777) == -1) {
            cerr << "mkdir failed " << dir << endl;
        } else {
            cout << "mkdir" << dir << endl;
        }
    }
}

// dump a part process state according to part type
void Car::SaveState(PartType part) {
    cout << __FUNCTION__ << " " << part_pid_[part] << endl;
    string dir = "./checkpoint/" + to_string(part_pid_[part]);
    // cout << "``````````````````````````" << dir << " " << part_pid_[part]
    // << endl;
    if (criu_init_opts() == -1) cout << "criu init failed" << endl;
    criu_set_pid(part_pid_[part]);
    int fd = open((char *)dir.c_str(), O_DIRECTORY);
    criu_set_images_dir_fd(fd);
    criu_set_leave_running(true);
    criu_set_shell_job(true);
    criu_set_log_file("dump.log");
    criu_set_log_level(4);
    criu_dump();
    close(fd);

    // string dir = "./checkpoint/" + to_string(part_pid_[part]);
    // char pid_buf[16];
    // sprintf(pid_buf, "%d", (int)part_pid_[part]);
    // char *cmd[] = {"sudo", "criu", "dump",  "-D", (char *)dir.c_str(),
    //                "-R",   "-t",   pid_buf, NULL};
    // pid_t pid = fork();
    // if (pid < 0) {
    //     //
    // } else if (pid == 0) {
    //     // child
    //     execvp("sudo", cmd);
    // } else {
    //     // parent
    //     // waitpid(pid,nullptr,WNOHANG);
    // }
}

// restore a part process state according to part type
void Car::RestoreState(PartType part) {
    cout << __FUNCTION__ << " " << part_pid_[part] << endl;
    string dir = "./checkpoint/" + to_string(part_pid_[part]);
    // cout << "``````````````````````````" << dir << " " << part_pid_[part]
    // << endl;
    if (criu_init_opts() == -1) cout << "criu init failed" << endl;
    int fd = open((char *)dir.c_str(), O_DIRECTORY);
    criu_set_images_dir_fd(fd);
    criu_set_shell_job(true);
    criu_set_log_file("resotre.log");
    criu_set_log_level(4);
    criu_restore();
    close(fd);

    // string dir = "./checkpoint/" + to_string(part_pid_[part]);
    // char pid_buf[16];
    // sprintf(pid_buf, "%d", (int)part_pid_[part]);
    // char *cmd[] = {"sudo", "criu", "restore", "-D", (char *)dir.c_str(),
    // NULL}; pid_t pid = fork(); if (pid < 0) {
    //     //
    // } else if (pid == 0) {
    //     // child
    //     execvp("sudo", cmd);
    // } else {
    //     // parent
    //     // waitpid(pid,nullptr,WNOHANG);
    // }
}

// simulate a fault for a part process according to part type
void Car::SimulateFalut(PartType part) {
    // // kill simulate attack
    // cout << "kill " << (int)part << endl;
    // char pid_buf[16];
    // sprintf(pid_buf, "%d", (int)part_pid_[part]);
    // char *cmd[] = {"sudo", "kill", "-s", "2", pid_buf, NULL};
    // pid_t p = fork();
    // if (p < 0) {
    //     cerr << "kill failed" << endl;
    // } else if (p == 0) {
    //     // child
    //     execvp("sudo", cmd);
    // } else {
    //     // parent
    //     wait(0);
    // }
    if (kill(part_pid_[part], SIGINT) == -1) {
        cerr << "kill " << part_pid_[part] << " failed" << endl;
    } else {
        cout << "kill " << part_pid_[part] << " successfully" << endl;
    }
}

// check if any part need a compulsive checkpoint
void Car::CheckCompulsiveCheckpoint() {
    DBG;
    for (int i = 0; i < PART_NUMBER; ++i) {
        if (state_->need_compulsive_checkpoint[i]) {
            cout << "SaveState compulsive " << (int)PartType(i) << endl;
            SaveState((PartType(i)));
            state_->need_compulsive_checkpoint[i] = false;
        }
    }
}

// check if any part need a unit checkpoint
void Car::CheckUnitCheckpoint() {
    DBG;
    timeval tv;
    gettimeofday(&tv, nullptr);
    for (int i = 0; i < PART_NUMBER; ++i) {
        if (tv.tv_usec - state_->last_unit_checkpoint[i].tv_usec >
            state_->average_time_per_cycle[i] / 2) {
            cout << "SaveState unit " << (int)PartType(i) << endl;
            SaveState((PartType(i)));
            state_->last_unit_checkpoint[i] = tv;
        }
    }
}

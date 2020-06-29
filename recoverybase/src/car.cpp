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
    // for (int i = 0; i < PART_NUMBER; ++i)
    //     cout << "part " << part_pid_[PartType(i)] << endl;

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

        state_->last_unit_checkpoint[i] = tv;
    }
    state_->average_time_per_cycle[0] = avgt0;
    state_->average_time_per_cycle[1] = avgt1;
    state_->average_time_per_cycle[2] = avgt2;
    state_->average_time_per_cycle[3] = avgt3;
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
            switch (pr.first) {
                case PartType::camera:
                    cout << "camera pid " << pid << endl;
                    break;
                case PartType::servoPID:
                    cout << "servopid pid " << pid << endl;
                    break;
                case PartType::throttlePID:
                    cout << "throttlepid pid " << pid << endl;
                    break;
                case PartType::speed:
                    cout << "speed pid " << pid << endl;
                    break;
                default:
                    break;
            }
        }
    }
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
    cout << __FUNCTION__ << " ing " << part_pid_[part] << endl;
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
    if (criu_dump() < 0) {
        cerr << "dump " << part_pid_[part] << " failed" << endl;
    } else {
        cout << "dump " << part_pid_[part] << " successfully" << endl;
    }
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
    cout << __FUNCTION__ << " ing " << part_pid_[part] << endl;
    string dir = "./checkpoint/" + to_string(part_pid_[part]);
    // cout << "``````````````````````````" << dir << " " << part_pid_[part]
    // << endl;
    if (criu_init_opts() == -1) cout << "criu init failed" << endl;
    int fd = open((char *)dir.c_str(), O_DIRECTORY);
    criu_set_images_dir_fd(fd);
    criu_set_shell_job(true);
    criu_set_log_file("resotre.log");
    criu_set_log_level(4);
    if (criu_restore() < 0) {
        cerr << "dump " << part_pid_[part] << " failed" << endl;
    } else {
        cout << "dump " << part_pid_[part] << " successfully" << endl;
    }
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
    // kill simulate attack
    if (kill(part_pid_[part], SIGINT) == -1) {
        cerr << "kill " << part_pid_[part] << " failed" << endl;
    } else {
        int statloc, opt;
        waitpid(part_pid_[part], &statloc, opt);
        cout << "kill " << part_pid_[part] << " successfully" << endl;
    }

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
}

// check if any part need a compulsive checkpoint
void Car::CheckCompulsiveCheckpoint() {
    DBG;
    for (int i = 0; i < PART_NUMBER; ++i) {
        if (state_->need_compulsive_checkpoint[i]) {
            // cout << "SaveState compulsive " << (int)PartType(i) << "-"
            //      << part_pid_[(PartType)i] << endl;
            // suppose a savestate() fucntion cost 0.01s
            timeval t1, t2;
            gettimeofday(&t1, nullptr);
#ifdef C
            SaveState((PartType(i)));
#endif
            gettimeofday(&t2, nullptr);
            total_run_time += diffus(t1, t2);
            state_->need_compulsive_checkpoint[i] = false;
        }
    }
}

// check if any part need a unit checkpoint
void Car::CheckUnitCheckpoint() {
    DBG;
    timeval tv;
#ifdef C
    int interval = 2;
#else
    int interval = 4;
#endif

    gettimeofday(&tv, nullptr);
    for (int i = 0; i < PART_NUMBER; ++i) {
        if (diffus(state_->last_unit_checkpoint[i], tv) >
            state_->average_time_per_cycle[i] / interval) {
            // cout << "SaveState unit " << (int)PartType(i) << "-"
            //      << part_pid_[(PartType)i] << endl;
            // 0.01s
            // cout << "!!!!!!!!!!!!!!!!!!!!!!!! " << i << " "
            //      << state_->average_time_per_cycle[i] << endl;
            timeval t1, t2;
            gettimeofday(&t1, nullptr);
            SaveState((PartType(i)));
            gettimeofday(&t2, nullptr);
            total_run_time += diffus(t1, t2);
            state_->last_unit_checkpoint[i] = tv;
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
    cout << "init last_time " << last_time << endl;
    while (true) {
        cout << "\n\n\n=============================== " << state_->times
             << " ===============================" << endl;
        CheckCompulsiveCheckpoint();
        CheckUnitCheckpoint();
        if (state_->times >= MAX_TIMES) break;
        if (last_time && last_time != state_->times &&
            state_->times % FAULT_STEP == 0) {
            last_time += FAULT_STEP;
            SimulateFalut(PartType::camera);
            // usleep(200);
            RestoreState(PartType::camera);
        }
        usleep(50);
    }
    gettimeofday(&t_end, nullptr);
    total_run_time = diffus(t_start, t_end);
    cout << "total run time " << total_run_time << " us"
         << " " << (double)total_run_time / 1e6 << " s" << endl;
}
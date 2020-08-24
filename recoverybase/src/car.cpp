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
    // ForkPart();
    // CreateStateDir();

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

    state_->is_new_data[0] = false;
    state_->is_new_data[1] = false;
    state_->is_new_data[2] = false;

    state_->need_compulsive_checkpoint[0] = false;
    state_->need_compulsive_checkpoint[1] = false;
    state_->need_compulsive_checkpoint[2] = false;

    state_->average_time_per_cycle[0] = avgt0;
    state_->average_time_per_cycle[1] = avgt1;
    state_->average_time_per_cycle[2] = avgt2;
    // state_->average_time_per_cycle[3] = avgt3;
}

// add all parts
void Car::AddPart() {
    DBG;
    part_pid_[PartType::camera] = -1;
    part_pid_[PartType::speed] = -1;
    part_pid_[PartType::servoPID] = -1;
    // part_pid_[PartType::throttlePID] = -1;
}

// create part process
void Car::ForkPart() {
    cout << __FUNCTION__ << endl;
    for (pair<PartType, pid_t> pr : part_pid_) {
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
                case PartType::speed:
                    part = new Speed();
                    break;
                case PartType::servoPID:
                    part = new ServoPID();
                    break;
                    // case PartType::throttlePID:
                    //     part = new ThrottlePID();
                    //     break;

                default:
                    break;
            }
            if (part == nullptr) {
                cerr << "fork part err" << endl;
            } else {
                cout << "start part" << (int)pr.first << endl;

                part->RunALL();
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
                // case PartType::throttlePID:
                //     cout << "throttlepid pid " << pid << endl;
                //     break;
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

    int count = 0;
    while (criu_restore() < 0 && count < 10) {
        cerr << "restore " << part_pid_[part] << " failed... retry... "
             << count++ << endl;
    }
    if (count < 10) {
        cout << "restore " << part_pid_[part] << " successfully" << endl;
    } else {
        cerr << "restore " << part_pid_[part] << " failed... " << endl;
        exit(0);
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
void Car::SimulateFalut() {
    // kill simulate attack

    // int rkill = kill(part_pid_[PartType::servoPID], SIGINT);
    // if (rkill == -1) {
    //     cerr << "kill " << part_pid_[PartType::servoPID] << " failed" <<
    //     endl;
    // } else {
    //     int statloc, opt = 0;
    //     waitpid(part_pid_[PartType::servoPID], &statloc, opt);
    //     cout << "kill " << part_pid_[PartType::servoPID] << " successfully"
    //          << endl;
    // }

    for (pair<PartType, pid_t> pr : part_pid_) {
        int rkill = kill(pr.second, SIGINT);
        if (rkill == -1) {
            cerr << "kill " << pr.second << " failed" << endl;
        } else {
            int statloc, opt = 0;
            waitpid(pr.second, &statloc, opt);
            cout << "kill " << pr.second << " successfully" << endl;
        }
    }
    // sleep(10);

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
            // timeval t1, t2;
            // gettimeofday(&t1, nullptr);

            // 1 only unit, 2 only compulsive, 3 both
            switch (METHOD) {
                case 1:
                    // SaveState((PartType(i)));
                    // gettimeofday(&t2, nullptr);
                    // total_run_time += diffus(t1, t2);
                    break;
                case 2:
                    SaveState((PartType(i)));
                    // gettimeofday(&t2, nullptr);
                    // total_run_time += diffus(t1, t2);
                    break;
                case 3:
                    SaveState((PartType(i)));
                    // gettimeofday(&t2, nullptr);
                    // total_run_time += diffus(t1, t2);
                    break;
            }

            state_->need_compulsive_checkpoint[i] = false;
        }
    }
}

// check if any part need a unit checkpoint
void Car::CheckUnitCheckpoint() {
    DBG;
    timeval tv;
    // #ifdef C
    //     int interval = 2;
    // #else
    //     int interval = 4;
    // #endif
    // int interval = 2;

    gettimeofday(&tv, nullptr);
    for (int i = 0; i < PART_NUMBER; ++i) {
        if (diffus(state_->last_unit_checkpoint[i], tv) >
            state_->average_time_per_cycle[i] / INTERVAL) {
            // cout << "SaveState unit " << (int)PartType(i) << "-"
            //      << part_pid_[(PartType)i] << endl;
            // 0.01s
            // cout << "!!!!!!!!!!!!!!!!!!!!!!!! " << i << " "
            //      << state_->average_time_per_cycle[i] << endl;
            // timeval t1, t2;
            // gettimeofday(&t1, nullptr);

            // 1 only unit, 2 only compulsive, 3 both
            switch (METHOD) {
                case 1:
                    SaveState((PartType(i)));
                    // gettimeofday(&t2, nullptr);
                    // total_run_time += diffus(t1, t2);
                    break;
                case 2:
                    //  gettimeofday(&t2, nullptr);
                    // gettimeofday(&t2, nullptr);
                    // total_run_time += diffus(t1, t2);
                    break;
                case 3:
                    SaveState((PartType(i)));
                    // gettimeofday(&t2, nullptr);
                    // total_run_time += diffus(t1, t2);
                    break;
            }

            state_->last_unit_checkpoint[i] = tv;
        }
    }
}

// run the car manager run loop
// moniter checkpoint
void Car::Run() {
    DBG;

    timeval tv;
    gettimeofday(&tv, nullptr);

    ForkPart();
    CreateStateDir();

    for (int i = 0; i < PART_NUMBER; ++i) {
        state_->is_new_data[i] = false;
        state_->need_compulsive_checkpoint[i] = false;

        state_->last_unit_checkpoint[i] = tv;
    }

    timeval t_start, t_end;
    gettimeofday(&t_start, nullptr);
    int last_time = state_->times;
    cout << "init last_time " << last_time << endl;

    bool faluted = FAULTED;
    while (state_->times <= MAX_TIMES) {
        // cout << "\n\n\n=============================== " << state_->times
        //      << " ===============================" << endl;

        CheckUnitCheckpoint();
        CheckCompulsiveCheckpoint();

        // if (state_->times >= MAX_TIMES) break;

        // last_time = state_->times;
        // if (last_time && last_time != state_->times &&
        //     state_->times > FAULT_STEP && !faluted) {
        if (state_->times > FAULT_STEP && faluted == false) {
            faluted = true;
            // SimulateFalut(PartType::servoPID);
            SimulateFalut();

            RestoreState(PartType::camera);
            RestoreState(PartType::speed);
            RestoreState(PartType::servoPID);

            // RestoreState(PartType::throttlePID);
        }
        // usleep(10);
    }
    gettimeofday(&t_end, nullptr);
    total_run_time = diffus(t_start, t_end);
    total_run_time *= MAX_TIMES * 1.0 / state_->times;
    cout << "total run time " << total_run_time << " us"
         << " " << (double)total_run_time / 1e6 << " s" << endl;
}
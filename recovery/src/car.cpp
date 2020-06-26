#include "car.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <iostream>

#include "camera.h"
#include "criu.h"
#include "part.h"
#include "pca.h"
#include "servoPID.h"
#include "speed.h"
#include "throttlePID.h"

using namespace std;

Car::Car() {
    cout << __FUNCTION__ << endl;
    Init();
}

void Car::Init() {
    cout << __FUNCTION__ << endl;
    InitState();
    AddPart();
    CreateStateDir();
    ForkPart();
    cout << "init finished" << endl;
}

void Car::InitState() {
    state = State::ShareMemoryInit();
    state->speed_ = 0;
    state->direction_ = 0;
    state->target_speed_ = 0;
    state->target_direction_ = 0;
    state->pca_ = new Pca();
}

void Car::AddPart() {
    part_pid[PartType::camera] = -1;
    part_pid[PartType::servoPID] = -1;
    part_pid[PartType::throttlePID] = -1;
    part_pid[PartType::speed] = -1;
}

void Car::AddHeatbeat() {
    // state->part_heartbeat["camera"] = time(NULL);
    //
}

void Car::ForkPart() {
    cout << __FUNCTION__ << endl;
    for (pair<PartType, time_t> pr : part_pid) {
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
                case PartType::pca:
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
                part->Run();
                cout << "start part" << (int)pr.first << endl;
            }
        } else {
            // car
            part_pid[pr.first] = pid;
            cout << pid << endl;
            // waitpid(pid, nullptr, WNOHANG);
        }
    }
}

void Car::Run() {
    int i = 0;
    while (true) {
        for (pair<PartType, pid_t> pr : part_pid) {
            if (i < 5) {
                SaveState(pr.first);
            } else if (i == 5) {
                // kill simulate attack
                cout << "kill " << pr.second << endl;
                char pid_buf[16];
                sprintf(pid_buf, "%d", (int)pr.second);
                char *cmd[] = {"sudo", "kill", "-s", "2", pid_buf, NULL};
                pid_t p = fork();
                if (p < 0) {
                    // err
                } else if (p == 0) {
                    // child
                    // sleep(5);
                    execvp("sudo", cmd);
                } else {
                    // parent
                    wait(0);
                }
            } else if (i == 10) {
                RestoreState(pr.first);
            }
        }
        sleep(1);
        ++i;
    }
}

void Car::CreateStateDir() {
    cout << __FUNCTION__ << endl;
    mkdir("checkpoint", 0777);
    for (pair<PartType, pid_t> pr : part_pid) {
        string dir = "./checkpoint/" + to_string((int)pr.first);
        if (mkdir(dir.c_str(), 0777) == -1) {
            cerr << "mkdir failed " << (int)pr.first << endl;
        }
    }
}

void Car::SaveState(PartType part) {
    cout << __FUNCTION__ << endl;
    string dir = "./checkpoint/" + to_string((int)part);
    if (criu_init_opts() == -1) cout << "criu init failed" << endl;
    criu_set_pid(part_pid[part]);
    int fd = open((char *)dir.c_str(), O_DIRECTORY);
    criu_set_images_dir_fd(fd);
    criu_set_leave_running(true);
    criu_set_shell_job(true);
    criu_set_log_file("dump.log");
    criu_set_log_level(4);
    criu_dump();
}

void Car::RestoreState(PartType part) {
    cout << __FUNCTION__ << endl;
    string dir = "./checkpoint/" + to_string((int)part);
    if (criu_init_opts() == -1) cout << "criu init failed" << endl;
    int fd = open((char *)dir.c_str(), O_DIRECTORY);
    criu_set_images_dir_fd(fd);
    criu_set_shell_job(true);
    criu_set_log_file("resotre.log");
    criu_set_log_level(4);
    criu_restore();
}
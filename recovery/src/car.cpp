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

using namespace std;

Car::Car() {
    cout << __FUNCTION__ << endl;
    state = State::ShareMemoryInit();
    state->speed_ = 0;
    state->img_ = 0;
    state->part_heartbeat[0] = time(NULL);
    // state->part_heartbeat["camera"] = time(NULL);
    part_pid["camera"] = -1;
    Init();
}

void Car::Init() {
    cout << __FUNCTION__ << endl;
    CreateStateDir();
    ForkPart();
    cout << "init finished" << endl;
}

void Car::ForkPart() {
    cout << __FUNCTION__ << endl;
    for (pair<string, time_t> pr : part_pid) {
        pid_t pid = fork();
        if (pid < 0) {
            // err
            cerr << "part fork err" << endl;
        } else if (pid == 0) {
            // part
            Part *part = nullptr;
            if (pr.first == "camera") {
                part = new Camera();
            }
            if (part == nullptr) {
                cerr << "fork part err" << endl;
            } else {
                part->Run();
                cout << "start part" << pr.first << endl;
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
        cout << "car" << state->img_ << endl;
        for (pair<string, pid_t> pr : part_pid) {
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
    for (pair<string, pid_t> pr : part_pid) {
        string dir = "./checkpoint/" + pr.first;
        if (mkdir(dir.c_str(), 0777) == -1) {
            cerr << "mkdir failed " << pr.first << endl;
        }
    }
}

void Car::SaveState(string part_name) {
    cout << __FUNCTION__ << endl;
    string dir = "./checkpoint/" + part_name;
    char pid_buf[16];
    sprintf(pid_buf, "%d", (int)part_pid[part_name]);
    char *cmd[] = {"sudo", "criu", "dump",  "-D", (char *)dir.c_str(),
                   "-R",   "-t",   pid_buf, NULL};
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
    if (criu_init_opts() == -1) cout << "criu init failed" << endl;
    criu_set_pid(part_pid[part_name]);
    int fd = open((char *)dir.c_str(), O_DIRECTORY);
    criu_set_images_dir_fd(fd);
    criu_set_leave_running(true);
    criu_set_shell_job(true);
    criu_dump();
}

void Car::RestoreState(string part_name) {
    cout << __FUNCTION__ << endl;
    string dir = "./checkpoint/" + part_name;
    // char pid_buf[16];
    // sprintf(pid_buf, "%d", (int)part_pid[part_name]);
    char *cmd[] = {"sudo", "criu", "restore", "-D", (char *)dir.c_str(), NULL};
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
    if (criu_init_opts() == -1) cout << "criu init failed" << endl;
    int fd = open((char *)dir.c_str(), O_DIRECTORY);
    criu_set_images_dir_fd(fd);
    criu_set_shell_job(true);
    criu_restore();
}
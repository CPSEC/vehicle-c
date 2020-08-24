#include <unistd.h>
#include <iostream>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <typeinfo>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <chrono>
#include <math.h>

#include "remote.h"
#include "car.h"

#define MAX 1024
#define PORT 13244
#define HOST "192.168.1.3"
#define SA struct sockaddr

using namespace std;
using namespace chrono;

Remote::Remote() : Part(PartType::remote){
    struct sockaddr_in servaddr, cli;
    // socket create and varification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));
    
    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(HOST);
    servaddr.sin_port = htons(PORT);
    
    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
        printf("connected to the server..\n");
}

void Remote::WaitNeededNewData() {
    return;
}

void Remote::UpdateIsNewData() {
    state_->is_new_data[3] = true;
    state_->is_new_data[4] = true;
    state_->is_new_data[5] = true;
    return;
}

void Remote::Run() {
    //Initialize the dicts
    for(int i = 0; i<33; i++){
        s_dict[(header[i])] = d[i];
    }
    p_dict["rspeed"] = 40;
    p_dict["mp"] = 3;
    p_dict["mi"] = 4;
    p_dict["md"] = 5;
    p_dict["sp"] = 6;
    p_dict["si"] = 7;
    p_dict["sd"] = 8;
    
    
    char buff[MAX];
    fd_set rfds, wfds;
    struct timeval timeout={0,0};
    auto cur = system_clock::now().time_since_epoch();// get time now
    auto t20mils = milliseconds(20); // 20 milliseconds
    auto t50mics = microseconds(50); // 50 microseconds
    auto nxt = cur + t50mics;
    double count = 0.0;
    
    cout << "Initialized" << endl;
    
    while(1){
        FD_ZERO(&rfds);
        FD_ZERO(&wfds);
        FD_SET(sockfd,&rfds);
        FD_SET(sockfd,&wfds);
        
        switch (select(sockfd+1, &rfds, &wfds, NULL, &timeout)) {
            case -1:
                cout << "case -1" << endl;
                exit(-1);
                break;
                
            case 0:
                cout << "case 0" << endl;
                break;
                
            default:
                //read from server
                if(FD_ISSET(sockfd, &rfds)){
                    //cout << "R-isset" << endl;
                    bzero(buff, sizeof(buff));
                    read(sockfd, buff, sizeof(buff));
                    //apply
                    //cout << "read" << endl;
                    reader.parse(buff, root);
                    state_->auto_ = (root["auto"].asString()=="true");
                    state_->direction_ = root["servo"].asDouble();
                    bool forward = (root["forward"].asString()=="true");
                    double throttle = root["throttle"].asDouble();
                    state_->speed_ = root["speed"].asDouble();
                    state_->servoPID_[0] = root["servo-PID"]["P"].asDouble();
                    state_->servoPID_[1] = root["servo-PID"]["I"].asDouble();
                    state_->servoPID_[2] = root["servo-PID"]["D"].asDouble();
                    state_->throttlePID_[0] = root["throttle-PID"]["P"].asDouble();
                    state_->throttlePID_[1] = root["throttle-PID"]["I"].asDouble();
                    state_->throttlePID_[2] = root["throttle-PID"]["D"].asDouble();
                }
                //write to server
                if(FD_ISSET(sockfd, &wfds)){
                    cur = system_clock::now().time_since_epoch();
                    //cout << "W-isset" << endl;
                    bzero(buff, sizeof(buff));
                    //greather data
                    
                    if(cur >= nxt){ // cur later then nxt
                        s_dict["milliseconds"] = 0.01 * count;
                        s_dict["as5048a"] = 30 * sin(0.1 * count) + 30;
                        //{"sensor":dict(zip(header,d)), "parameter": p_dict, "image": img.tolist()}
                        dict["sensor"] = s_dict;
                        dict["parameter"] = p_dict;
                        
                        //cout<< s_dict << endl;
                        //cout<< dict << endl;
                        
                        memset(buff, 0, sizeof(buff));
                        
                        // output Json
                        Json::FastWriter fastWriter;
                        string output = fastWriter.write(dict);
                        output = output.substr(0,output.size()-1)+ sep; // strip '\n'
                        strcpy(buff, output.c_str());
                        
                        // write to socket
                        write(sockfd, buff, sizeof(buff));
                        
                        nxt = nxt + t20mils;
                        count += 1;
                    }
                }
                break;
        }
    }
}


Remote::~Remote() {
    close(sockfd);
}

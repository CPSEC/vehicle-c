#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#define MAX 80
#define PORT 8080
#define SA struct sockaddr

#include "remote.h"
#include "car.h"

#include <jsoncpp/json/json.h>

#include <unistd.h>

#include <iostream>


using namespace std;

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
    servaddr.sin_addr.s_addr = inet_addr("192.168.1.5");
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
    char buff[MAX];
    int n;
    for (;;) {
        bzero(buff, sizeof(buff));
        printf("Enter the string : ");
        n = 0;
        while ((buff[n++] = getchar()) != '\n')
            ;
        write(sockfd, buff, sizeof(buff));
        bzero(buff, sizeof(buff));
        read(sockfd, buff, sizeof(buff));

        //create json reader
        Json::Reader reader;
        //value saved in root
        Json::Value root;
        
        //read from buff and save it in root
        reader.parse(buff, root);
        
        //cast results into desired type
        state_->auto_ = root["auto"].asBool();
        state_->direction_ = root["servo"].asDouble();
        bool forward = root["forward"].asBool();
        double throttle = root["throttle"].asDouble();
        state_->speed_ = root["speed"].asDouble();
        state_->servoPID_[0] = root["servo-PID"]["P"].asDouble();
        state_->servoPID_[1] = root["servo-PID"]["I"].asDouble();
        state_->servoPID_[2] = root["servo-PID"]["D"].asDouble();
        state_->throttlePID_[0] = root["throttle-PID"]["P"].asDouble();
        state_->throttlePID_[1] = root["throttle-PID"]["I"].asDouble();
        state_->throttlePID_[2] = root["throttle-PID"]["D"].asDouble();

    
        /////////////////////////////////
        printf("From Server : %s", buff);
        if ((strncmp(buff, "exit", 4)) == 0) {
            printf("Client Exit...\n");
            break;
        }
    }
}

Remote::~Remote() {
    close(sockfd);
}

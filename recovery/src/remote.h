#ifndef _REMOTE_H
#define _REMOTE_H

#include "part.h"
#include <string.h>
#include <jsoncpp/json/json.h>

class Remote : public Part {
public:
    int sockfd, connfd;
    
    // seprate symbol
    string sep = "\x1e";
    
    // initialize Json type
    Json::Reader reader;
    Json::FastWriter fastWriter;
    Json::Value root;
    
    Json::Value s_dict;//sensor dictionary
    Json::Value p_dict;//parameter dictionary
    Json::Value dict;//sending dictionary
    
    // key value pair for sensor
    string header[33] = {"milliseconds", "rspeed", "as5048a", "throttle", "vm", "angle", "bias", "radius", "hcsr04", "vp", "heading", "roll",
        "pitch", "ori_x", "ori_y", "ori_z", "ori_w", "temp_c", "mag_x", "mag_y", "mag_z", "gyr_x", "gyr_y", "gyr_z",
        "acc_x", "acc_y", "acc_z", "lacc_x", "lacc_y", "lacc_z", "gra_x", "gra_y", "gra_z"};
    double d[33] = {0.737365723, 40, 106.7478557, 0.7, 8.167968, -0.5670339, 5.1, 30.5, 123.258422762904, 8.195808, 275.25, -3,
        6.3125, -0.05908203125, -0.01763916, 0.672607422, 0.7374267578125, 23, 24.25, 7.5, -46.75, 0, 0.001111111,
        0.001111111, -0.49, -0.93, 8.46, 0.05, 0.12, -1.26, -0.52, -1.08, 9.73};
    
    Remote();
    virtual void WaitNeededNewData() override;
    virtual void UpdateIsNewData() override;
    void Run() override;
    ~Remote();
};

#endif


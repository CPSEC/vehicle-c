#include <bits/stdc++.h>
#include <iostream>
#include <cstdlib>
#include <chrono>
using namespace std::chrono;
#include "as5048a.h"
using namespace std;
#include "speed.h"

Speed::Speed(Car* car){
    name_ = "Speed thread";
    car_ = car;
    is_thread_ = true;
}

void Speed::Run(){
    //initial
    int result1 = 0;
    int result2 = 0;
    int difference = 0;
    int difference_total = 0;
    double time_taken = 0.0;
    double time_total = 0.0;
    double speed = 0.0;
    //clear error
    As5048a chalega_kya(0,128);
    result1 = chalega_kya.Read(AS5048_CMD_CLEAR_ERROR);
    sleep(1);
    //start looping
    while(1){
        //initial loop
        difference_total = 0;
        time_total = 0.0;
        //take average of 8 sample
        for(int i = 1; i <=8; i++){
            //get angle and time of start point
            result1 = chalega_kya.Read(AS5048_CMD_ANGLE);
            auto start = high_resolution_clock::now();
            //get angle and time of end point
            result2 = chalega_kya.Read(AS5048_CMD_ANGLE);
            auto stop = high_resolution_clock::now();
            
            //filter negative value
            if(result1 < 0 || result2 < 0){
                i--;
                continue;
            }
            
            //calc the duration between two samples
            auto duration = duration_cast<microseconds>(stop - start);
            //time diff in second
            time_taken = double(duration.count()) / 1000000.0;
            
            //filter missed deadline result
            if(time_taken > 0.0025 || time_taken < 0.0){
                i--;
                continue;
            }
            
            //inverse degree diff
            difference = (result2 - result1) * (-1);
            
            //resolve passing 0 degree point
            if(difference < -10467) difference += 16383;
            else if (difference > 10467) difference -= 16383;
            
            //sum degree difference, time difference
            difference_total += difference;
            time_total += time_taken;
        }
        //calc the speed
        speed = double(difference_total) / 16383.0 / time_total;
        
        if(result1 == -1 || result2 == -1){
            result1 = chalega_kya.Read(AS5048_CMD_CLEAR_ERROR);
            result2 = result1;
        }
        else{
            car_->speed = speed;
            usleep(50000);
        }
        
    }
}

void Speed::StartSeq(){
    
}

void Speed::StartThread(){
    cout << name_ << endl;
    thread_ = new thread(&Speed::Run, this);
    thread_ -> detach();
}

Speed::~Speed(){
    
}



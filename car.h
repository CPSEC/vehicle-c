#ifndef _CAR_H
#define _CAR_H

#include <iostream>
#include <string>
#include <unordered_map>

using namespace std;

class Part;

// example
class Camera;

class Car {
   public:
    int test;
    double speed;
    double direction;
    double target_direction;
    double target_speed;
    unordered_map<string, Part *> thread_parts_;
    unordered_map<string, Part *> seq_parts_;

    Car();
    Car operator=(Car &&) = delete;
    void Init();

    void AddPart(Part *p);
    void StartThread();
    void StartSequence();
    void StartAll();
};

#endif

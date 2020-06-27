#ifndef _SPEED_H
#define _SPEED_H

#include "car.h"
#include "part.h"

class Speed : public Part {
   public:
    int result1;
    int result2;
    int difference;
    int difference_total;
    double time_taken;
    double time_total;
    double speed;

    Speed();
    void Run() override;
    ~Speed();
};

#endif

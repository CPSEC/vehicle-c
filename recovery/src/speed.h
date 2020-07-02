#ifndef _SPEED_H
#define _SPEED_H

#include "../lib/as5048a.h"
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
    As5048a chalega_kya;
    
    Speed();
    virtual void WaitNeededNewData() override;
    virtual void UpdateIsNewData() override;
    void Run() override;
    ~Speed();
};

#endif

#ifndef _ThrottlePID_H
#define _ThrottlePID_H

#include "../lib/MiniPID.h"
#include "part.h"

class ThrottlePID : public Part {
   public:
    MiniPID pid;
    ThrottlePID();
    virtual void WaitNeededNewData() override;
    virtual void UpdateIsNewData() override;
    void Run() override;
    ~ThrottlePID();
};

#endif

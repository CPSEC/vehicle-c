#ifndef _ThrottlePID_H
#define _ThrottlePID_H

#include "part.h"

class ThrottlePID : public Part {
   public:
    ThrottlePID();
    virtual void WaitNeededNewData() override;
    virtual void UpdateIsNewData() override;
    virtual void Run() override;
    ~ThrottlePID();
};

#endif

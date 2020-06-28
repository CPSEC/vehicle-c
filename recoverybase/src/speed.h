#ifndef _SPEED_H
#define _SPEED_H

#include "part.h"

class Speed : public Part {
   public:
    Speed();
    virtual void WaitNeededNewData() override;
    virtual void UpdateIsNewData() override;
    virtual void Run() override;
    virtual ~Speed();
};

#endif

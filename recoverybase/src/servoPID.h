#ifndef _SERVOPID_H
#define _SERVOPID_H

#include "part.h"

class ServoPID : public Part {
   public:
    ServoPID();
    virtual void WaitNeededNewData() override;
    virtual void UpdateIsNewData() override;
    virtual void Run() override;
    virtual ~ServoPID();
};

#endif

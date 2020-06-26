#ifndef _SERVOPID_H
#define _SERVOPID_H

#include "MiniPID.h"
#include "car.h"
#include "part.h"

class ServoPID : public Part {
   public:
    MiniPID pid;
    ServoPID();
    void Run() override;
    ~ServoPID() override;
};

#endif

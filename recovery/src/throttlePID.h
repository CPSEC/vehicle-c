#ifndef _ThrottlePID_H
#define _ThrottlePID_H

#include "../lib/MiniPID.h"
#include "part.h"

class ThrottlePID : public Part {
   public:
    MiniPID pid;
    ThrottlePID();
    void Run() override;
    ~ThrottlePID();
};

#endif

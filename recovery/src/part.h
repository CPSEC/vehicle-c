#ifndef _PART_H
#define _PART_H

#include <chrono>
#include <string>

#include "share.h"

class Part {
   public:
    PartType part_type_;
    State* state_;
    Part(PartType parttype);
    // for pca control
    // virtual void setSpeed(float pulse){};
    // virtual void setDirect(float pulse){};
    // virtual void setRGB(int rv, int gv, int bv){};
    // virtual void resetRGB(){};
    //
    virtual void Run();
    virtual ~Part();
};

#endif
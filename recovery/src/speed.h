#ifndef _SPEED_H
#define _SPEED_H

#include "car.h"
#include "part.h"

class Speed : public Part {
   public:
    Speed();
    void Run() override;
    ~Speed();
};

#endif

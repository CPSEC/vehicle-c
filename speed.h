#ifndef _SPEED_H
#define _SPEED_H

#include "part.h"
#include "car.h"

class Speed : public Part{
public:
    Speed(Car* car);
    void Run();
    virtual void StartSeq() override;
    virtual void StartThread() override;
    ~Speed();
};

#endif

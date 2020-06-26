#ifndef _ThrottlePID_H
#define _ThrottlePID_H

#include "part.h"
#include "car.h"
#include "MiniPID.h"

class ThrottlePID : public Part{
public:
    MiniPID pid = MiniPID(0.00228677780020377, 0.00277411402751383, -0.000169192834187225);
    ThrottlePID(Car* car);
    void Run();
    virtual void StartSeq() override;
    virtual void StartThread() override;
    ~ThrottlePID();
};

#endif


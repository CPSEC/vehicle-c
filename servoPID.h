#ifndef _SERVOPID_H
#define _SERVOPID_H

#include "part.h"
#include "car.h"
#include "MiniPID.h"

class ServoPID : public Part{
public:
    MiniPID pid = MiniPID(0.00228677780020377, 0.00277411402751383, -0.000169192834187225);
    ServoPID(Car* car);
    void Run();
    virtual void StartSeq() override;
    virtual void StartThread() override;
    ~ServoPID();
};

#endif

#ifndef _SPEED_H
#define _SPEED_H

#include "part.h"

class Speed : public Part{
public:
    Speed(Car* car, bool is_thread);
    void Run();
    virtual void StartSeq() override;
    virtual void StartThread() override;
    virtual ~Speed() override;
};

#endif

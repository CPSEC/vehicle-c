#ifndef _CAMERA_H
#define _CAMERA_H

#include "part.h"

class Camera : public Part {
   public:
    Camera();
    virtual void WaitNeededNewData() override;
    virtual void UpdateIsNewData() override;
    virtual void Run() override;

    virtual ~Camera();
};

#endif

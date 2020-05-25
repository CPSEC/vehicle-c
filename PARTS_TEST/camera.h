#ifndef _CAMERA_H
#define _CAMERA_H

#include "part.h"


class Camera : public Part {
   public:
    Camera(Car*car,bool is_thread);
    void Run(int test);
    virtual void StartSeq() override;
    virtual void StartThread() override;
    virtual ~Camera() override;
};

#endif
#ifndef _CAMERA_H
#define _CAMERA_H

#include "part.h"

class Camera : public Part {
   public:
    Camera();
    void Run() override;
    virtual ~Camera();
};

#endif

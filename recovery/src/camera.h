#ifndef _CAMERA_H
#define _CAMERA_H

#include "part.h"

class State;

enum class PartType : int;

class Camera : public Part {
   public:
    State* state;
    PartType part_type_;
    Camera();
    virtual void Run() override;
};

#endif
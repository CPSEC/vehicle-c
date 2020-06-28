#ifndef _PART_H
#define _PART_H

#include "share.h"

class Part {
   public:
    PartType part_type_;
    State* state_;
    Part(PartType parttype);
    virtual void SetCompulsiveCheckpoint();
    virtual void WaitCompulsiveCheckpoint();
    virtual void UpdateAverageCycleTime(timeval& tv_start, timeval& tv_end);
    virtual void WaitNeededNewData() = 0;
    virtual void UpdateIsNewData() = 0;
    virtual void Run() = 0;
    virtual void RunALL();
    virtual ~Part();
};

#endif
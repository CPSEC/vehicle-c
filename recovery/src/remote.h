#ifndef _REMOTE_H
#define _REMOTE_H

#include "part.h"


class Remote : public Part {
public:
    int sockfd, connfd;
    
    Remote();
    virtual void WaitNeededNewData() override;
    virtual void UpdateIsNewData() override;
    void Run() override;
    ~Remote();
};

#endif


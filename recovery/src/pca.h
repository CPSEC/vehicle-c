#ifndef _PCA_H
#define _PCA_H

#include "part.h"

class Pca {
   public:
    Pca();
    int calcTicks(float impulseMs, int hertz);
    virtual void setSpeed(float pulse);
    virtual void setDirect(float pulse);
    virtual void setRGB(int rv, int gv, int bv);
    virtual void resetRGB();
    ~Pca();
};

#endif

#ifndef _PCA_H
#define _PCA_H

#include "part.h"

class Pca : public Part{
public:
    Pca(Car* car);
    int calcTicks(float impulseMs, int hertz);
    virtual void setSpeed(float pulse) override;
    virtual void setDirect(float pulse) override;
    virtual void setRGB(int rv, int gv, int bv) override;
    virtual void resetRGB() override;
    virtual void StartSeq() override;
    virtual void StartThread() override;
    ~Pca();
};

#endif

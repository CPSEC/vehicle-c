#ifndef _PCA_H
#define _PCA_H

#include "part.h"

class Pca : public Part{
public:
    Pca(Car* car, bool is_thread);
    int calcTicks(float impulseMs, int hertz);
    void setSpeed(float pulse);
    void setDirect(float pulse);
    void setRGB(int rv, int gv, int bv);
    void resetRGB();
    virtual ~Pca() override;
};

#endif

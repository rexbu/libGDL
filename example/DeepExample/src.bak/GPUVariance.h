/**
 * file :	GPUVariance.h
 * author :	Rex
 * create :	2018-02-08 22:50
 * func :   标准差
 * history:
 */

#ifndef	__GPUVARIANCE_H_
#define	__GPUVARIANCE_H_

#include "GPUCNN.h"

class GPUVariance: public GPULayerBase{
public:
    GPUVariance();
    
    virtual void setFrameSize(uint32_t width, uint32_t height);
    virtual void activeOutFrameBuffer();
    
    virtual void diff();
};

#endif

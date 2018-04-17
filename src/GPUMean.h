/**
 * file :	GPUMean.h
 * author :	Rex
 * create :	2018-02-08 22:49
 * func : 
 * history:
 */

#ifndef	__GPUMEAN_H_
#define	__GPUMEAN_H_

#include "GPUCNN.h"

class GPUMean: public GPULayerBase{
public:
    GPUMean();
    
    virtual void setFrameSize(uint32_t width, uint32_t height);
    virtual void activeOutFrameBuffer();
    
    virtual void diff();
};

#endif

/**
 * file :	GPUColorFilter.h
 * author :	Rex
 * create :	2017-05-26 12:13
 * func : 
 * history:
 */

#ifndef	__GPUCONTRASTFILTER_H_
#define	__GPUCONTRASTFILTER_H_

#include "GPUFilter.h"
#include "GPUTwoPassFilter.h"

class GPUContrastFilter: public GPUFilter{
public:
    GPUContrastFilter();
};

class GPUBinaryFilter: public GPUFilter{
public:
    GPUBinaryFilter();
    
    virtual void setExtraParameter(float p);
};

class GPUSaltFilter: public GPUFilter{
public:
    GPUSaltFilter();
    
};

// 水平1*9卷积和垂直9*1卷积
class GPUHConvolution9Filter: public GPUTwoPassFilter{
public:
    GPUHConvolution9Filter();
    virtual void setFrameSize(uint32_t width, uint32_t height);
};

#endif

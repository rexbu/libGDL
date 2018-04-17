/**
 * file :	GPUBilateralBlurFilter.h
 * author :	Rex
 * create :	2017-05-26 10:34
 * func :   平滑滤镜
 * history:
 */

#ifndef	__GPUBILATERALBLURFILTER_H_
#define	__GPUBILATERALBLURFILTER_H_

#include "GPUTwoPassFilter.h"

class GPUBilateralBlurFilter: public GPUTwoPassFilter{
public:
    GPUBilateralBlurFilter();
    
    virtual void setInputFrameBuffer(GPUFrameBuffer *buffer, int location = 0);
    virtual void setExtraParameter(float strength);
protected:
    
};

#endif

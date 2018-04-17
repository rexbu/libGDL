/**
 * file :	GPUGaussianBlurFilter.h
 * author :	Rex
 * create :	2016-07-13 23:29
 * func :   高斯模糊
 * history:
 */

#ifndef	__GPUGAUSSIANBLURFILTER_H_
#define	__GPUGAUSSIANBLURFILTER_H_

#include "GPUTwoPassFilter.h"
#include "GPUMedianFilter.h"

class GPUGaussianBlurFilter: public GPUTwoPassFilter{
public:
    GPUGaussianBlurFilter(uint32_t radius=4, float sigma=2.0);
    
    virtual void setExtraParameter(float pixel);
    virtual void setInputFrameBuffer(GPUFrameBuffer *buffer, int location = 0);
    
protected:
    char* generateShader(uint32_t radius, float sigma);
    
    char        m_fragment[10240];
    
    uint32_t 	m_frame_width;
    uint32_t	m_frame_height;
};

#endif

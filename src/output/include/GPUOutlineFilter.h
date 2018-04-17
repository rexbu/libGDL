/**
 * file :	GPUOutlineFilter.h
 * author :	Rex
 * create :	2016-08-16 20:08
 * func :   轮廓检测
 * history:
 */

#ifndef	__GPUOUTLINEFILTER_H_
#define	__GPUOUTLINEFILTER_H_

#include "GPUSobelEdgeFilter.h"
#include "GPUGroupFilter.h"
#include "GPUGaussianBlurFilter.h"

class GPUOutlineFilter: public GPUGroupFilter{
public:
    GPUOutlineFilter();
    virtual void setInputFrameBuffer(GPUFrameBuffer *buffer, int location = 0);
    
protected:
    GPUGaussianBlurFilter   m_gaussian_filter;
    GPUSobelEdgeFilter  m_sobel_filter;
    GPUFilter           m_outline_filter;

    uint32_t			m_frame_width;
    uint32_t			m_frame_height;
};

#endif

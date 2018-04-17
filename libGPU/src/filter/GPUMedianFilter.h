/**
 * file :	GPUMedianFilter.h
 * author :	Rex
 * create :	2016-08-05 15:05
 * func :   中值滤波
 * history:
 */

#ifndef	__GPUMEDIANFILTER_H_
#define	__GPUMEDIANFILTER_H_

#include "GPUFilter.h"

class GPUMedianFilter: public GPUFilter{
public:
    GPUMedianFilter();
    
    virtual void setFrameSize(uint32_t width, uint32_t height);
};

class GPUMeansFilter: public GPUFilter{
public:
    GPUMeansFilter();
    
    virtual void setFrameSize(uint32_t width, uint32_t height);
};

/*
class GPUMedianFilter:public GPUTwoPassFilter{
public:
    GPUMedianFilter(uint32_t distance = 2);
    
    void setPixelRadius(uint32_t distance);
    
    virtual void setInputFrameBuffer(GPUFrameBuffer *buffer, int location = 0);
    
protected:
    char* generateShader(uint32_t distance);
    
    char        m_fragment[10240];
    
    uint32_t 	m_frame_width;
    uint32_t	m_frame_height;
};
 */
#endif

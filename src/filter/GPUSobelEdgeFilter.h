/**
 * file :	GPUSobelEdgeFilter.h
 * author :	Rex
 * create :	2016-08-10 20:57
 * func : 
 * history:
 */

#ifndef	__GPUSOBELEDGEFILTER_H_
#define	__GPUSOBELEDGEFILTER_H_

#include "GPUTwoPassFilter.h"
#include "GPULuminanceFilter.h"

class GPUSobelEdgeFilter: public GPUTwoPassFilter{
public:
    GPUSobelEdgeFilter(float strength = 1.0);
    
    virtual void setInputFrameBuffer(GPUFrameBuffer *buffer, int location = 0);
    virtual void setExtraParameter(float p);
    void setEdgeThreshold(float t);
    
protected:
    uint32_t 	m_frame_width;
    uint32_t	m_frame_height;
};

class GPUSobelDirectionFilter: public GPUFilter{
public:
    GPUSobelDirectionFilter(float strength = 1.0);
    
    virtual void setExtraParameter(float p);
    virtual void setInputFrameBuffer(GPUFrameBuffer *buffer, int location = 0);
};

class GPUDirectionFilter: public GPUFilter{
public:
    GPUDirectionFilter();
    
    virtual void setExtraParameter(float p);
    virtual void setFrameSize(uint32_t width, uint32_t height);
};

#pragma --mark "笔画细化"
class GPUStrokeFilter: public GPUFilter{
public:
    GPUStrokeFilter(int num = 1);
    
    virtual void setFrameSize(uint32_t width, uint32_t height);
    virtual void render();
    
    // 迭代次数
    virtual void setIterations(int num);
    
protected:
    uint32_t        m_iternum;
};
#endif

/**
 * file :	GPUBlend2Filter.h
 * author :	Rex
 * create :	2017-04-06 17:21
 * func : 
 * history:
 */

#ifndef	__GPUBLEND2FILTER_H_
#define	__GPUBLEND2FILTER_H_

#include "GPUFilter.h"
#include "GPUPicture.h"

class GPUBlend2Filter: public GPUFilter{
public:
    GPUBlend2Filter();
    ~GPUBlend2Filter();
    
    // 设置要进行blend的图片
    void setBlendImage(GPUPicture* image, gpu_rect_t rect, bool mirror);
    void setBlendImagePoints(GPUPicture* pic, gpu_point_t points[4], bool mirror);
    
    virtual void setInputFrameBuffer(GPUFrameBuffer *buffer, int location=0);
protected:
    
    float       m_coordinates[8];  // texture坐标
    GPUPicture* m_blend_pic;
};

#endif

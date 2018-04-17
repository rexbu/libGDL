/**
 * file :	GPUBlendImageFilter.h
 * author :	Rex
 * create :	2017-04-14 16:27
 * func : 
 * history:
 */

#ifndef	__GPUBLENDIMAGEFILTER_H_
#define	__GPUBLENDIMAGEFILTER_H_

#include "GPUBlendFilter.h"
#include "GPUPicture.h"

class GPUBlendImageFilter: public GPUBlendFilter{
public:
    GPUBlendImageFilter(int image_num=1);
    void setBlendImageRect(GPUPicture* pic, gpu_rect_t rect, int index=0);
    void setBlendImage(GPUPicture* pic, gpu_point_t points[4], int index=0);
    
    virtual void setInputFrameBuffer(GPUFrameBuffer *buffer, int location = 0);
protected:
    std::vector<GPUPicture*>       m_images;
};

#endif

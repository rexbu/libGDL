/**
 * file :	GPUZoomFilter.h
 * author :	Rex
 * email : rex@labjk.com
 * create :	2016-07-12 20:33
 * func : 
 * history:
 */

#ifndef	__GPUZOOMFILTER_H_
#define	__GPUZOOMFILTER_H_

#include "GPUFilter.h"

class GPUZoomFilter:public GPUFilter{
public:
    GPUZoomFilter(gpu_fill_mode_t mode=GPUFillModePreserveAspectRatioAndFill);
    virtual void render();
    // 只支持RGBA和BGRA
    void setOutputFormat(gpu_pixel_format_t format);
};

#endif

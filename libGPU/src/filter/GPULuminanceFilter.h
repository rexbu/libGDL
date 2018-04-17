/**
 * file :	GPULuminanceFilter.h
 * author :	Rex
 * create :	2016-08-05 23:08
 * func : 
 * history:
 */

#ifndef	__GPULUMINANCERANGEFILTER_H_
#define	__GPULUMINANCERANGEFILTER_H_

#include "GPUFilter.h"
#include "GPUGroupFilter.h"
#include "GPUGaussianBlurFilter.h"
#include "GPUSaturationFilter.h"

// 转灰度图
class GPULuminanceFilter: public GPUFilter{
public:
    GPULuminanceFilter();
};

class GPULuminanceRangeFilter: public GPUFilter{
public:
    GPULuminanceRangeFilter(float rangeReduction = 0.6);
    virtual void setExtraParameter(float r);
};

class GPUExtraBlurFilter: public GPUGroupFilter{
public:
    GPUExtraBlurFilter();
    virtual void setExtraParameter(float r);
protected:
    GPUSaturationFilter         m_saturation_filter;
    GPUGaussianBlurFilter       m_gaussian_filter;
    GPULuminanceRangeFilter     m_luminance_filter;
};
#endif

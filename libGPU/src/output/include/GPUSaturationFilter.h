/**
 * file :	GPUSaturationFilter.h
 * author :	Rex
 * create :	2016-08-07 11:12
 * func :   饱和度
 * history:
 */

#ifndef	__GPUSATURATIONFILTER_H_
#define	__GPUSATURATIONFILTER_H_

#include "GPUFilter.h"

class GPUSaturationFilter:public GPUFilter{
public:
    GPUSaturationFilter(float m_saturation = 1.0f);
    
    virtual void setExtraParameter(float saturation);
protected:
    float   m_saturation;
};
#endif

/**
 * file :	GPUSmoothFilter.h
 * author :	Rex
 * create :	2016-12-06 23:16
 * func : 
 * history:
 */

#ifndef	__GPUSMOOTHFILTER_H_
#define	__GPUSMOOTHFILTER_H_

#include "GPUFilter.h"

class GPUSmoothFilter: public GPUFilter{
public:
    GPUSmoothFilter();
    virtual void setExtraParameter(float p);
    /// 0, 0.54, 0.69, 0.79, 0.90, 0.99分别对应0、1、2、3、4、5
    void setSmoothLevel(int level);

protected:
	float 		m_strength;
};

#endif

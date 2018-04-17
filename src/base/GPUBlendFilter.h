/**
 * file :	GPUBlendFilter.h
 * author :	Rex
 * email : rex@labjk.com
 * create :	2016-07-29 14:36
 * func : 
 * history:
 */

#ifndef	__GPUMBLENDFILTER_H_
#define	__GPUMBLENDFILTER_H_

#include "GPUFilter.h"
#include "GPUPicture.h"

class GPUBlendFilter:public GPUFilter{
public:
    GPUBlendFilter(int inputs=2);
    GPUBlendFilter(const char* fragment, int inputs=2);
    GPUBlendFilter(const char* vertex, const char* fragment, int inputs=2);
    
    virtual void newFrame();
    
    // 设置第几个texture的四角坐标，顺序 lt/rt/lb/rb
    void setDrawRect(gpu_point_t points[4], int i);
    
    const static int GPU_MUTABLE_TEXTURE_NUM = 8;
    const static char* g_mutable_vertext_shader;
    const static char* g_mutable_fragment_shader;
    
protected:
    float       m_coordinates[GPU_MUTABLE_TEXTURE_NUM][8];  // texture坐标
};
#endif

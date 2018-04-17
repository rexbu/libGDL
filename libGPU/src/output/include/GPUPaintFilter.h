/**
 * file :	GPUPaintFilter.h
 * author :	Rex
 * create :	2017-05-23 13:25
 * func :   基于GPUBezierFilter画多段不连续曲线
 * history:
 */

#ifndef	__GPUPAINTFILTER_H_
#define	__GPUPAINTFILTER_H_

#include "GPUBezierFilter.h"

class GPUPaintFilter: public GPUFilter{
public:
    GPUPaintFilter(int w=720, int h=1280);
    ~GPUPaintFilter();
    
    void clear();
    // 添加一条曲线
    void addPoints(gpu_point_t* points, int n);
    
    // c0-c1颜色渐变
    void setColors(gpu_colorf_t c0, gpu_colorf_t c1);
    void setLineWidth(uint32_t w){ m_line_width = w; }
    
    virtual void newFrame();
    
protected:
    std::vector<GPUBezierFilter*>    m_bezier_filters;
    
    gpu_colorf_t        m_c0;
    gpu_colorf_t        m_c1;
    uint32_t            m_line_width;
};

#endif

/**
 * file :	GPULineFilter.h
 * author :	Rex
 * email : rex@labjk.com
 * create :	2017-05-19 15:17
 * func : 
 * history:
 */

#ifndef	__GPULINEFILTER_H_
#define	__GPULINEFILTER_H_

#include "GL.h"
#include "GPUFilter.h"

class GPULineFilter:public GPUFilter{
public:
    GPULineFilter(int width=720, int height=1280);
    
    virtual void initShader();
    virtual void render();
    
    // 线段宽度，像素数
    void setLineWidth(uint32_t width);
    virtual void setPoints(gpu_point_t* points, int n);
    virtual void setColors(gpu_colorf_t* color, int n);
    virtual void setColor(gpu_colorf_t* color, int i);
    
    // 利用360个点画三角形形成圆
    void drawPoint(float x, float y, uint32_t radius, float* color);
    // 利用360个点和圆心画扇形形成圆
    void drawRound(float x, float y, uint32_t radius, float* color);
    
protected:
    GLuint      m_color;
    uint32_t    m_line_width;   // 线条宽度，像素宽度
    
    std::vector<float>      m_points;
    std::vector<float>      m_colors;
    uint32_t                m_vertex_count;
};

#endif

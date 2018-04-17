/**
 * file :	GPUBezierFilter.h
 * author :	Rex
 * create :	2017-05-22 16:50
 * func :   贝塞尔曲线
 * history:
 */

#ifndef	__GPUBEZIERFILTER_H_
#define	__GPUBEZIERFILTER_H_

#include "GPULineFilter.h"

// 三点贝塞尔曲线
class GPUBezierFilter: public GPULineFilter{
public:
    GPUBezierFilter(int w=720, int h=1280):GPULineFilter(w, h){};
    // 此处p1不是控制点，是要经过的点，利用三点猜测控制点坐标
    GPUBezierFilter(gpu_point_t p0, gpu_point_t p1, gpu_point_t p2);
    
    // 贝塞尔曲线的3个端点，p1不是控制点，是要经过的点，利用三点猜测控制点坐标
    void setPoints(gpu_point_t p0, gpu_point_t p1, gpu_point_t p2);
    // 穿过多个点画曲线
    virtual void setPoints(gpu_point_t* points, int n);
    // 从c0-c1，颜色渐变
    void setColors(gpu_colorf_t c0, gpu_colorf_t c1);
    
protected:
    void calSegmentPoints(gpu_point_t p0, gpu_point_t p1, gpu_point_t p2, std::vector<gpu_point_t>& segments);
    
    gpu_point_t                 m_vertex_points[3];   // 贝塞尔曲线的3个端点
    
    gpu_point_t                 m_control_point;
    uint32_t                    m_segment;
};

#endif

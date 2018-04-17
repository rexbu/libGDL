/**
 * file :	GPUBezierFilter.cpp
 * author :	Rex
 * create :	2017-05-22 16:50
 * func : 
 * history:
 */

#include "GPUBezierFilter.h"

GPUBezierFilter::GPUBezierFilter(gpu_point_t p0, gpu_point_t p1, gpu_point_t p2){
    setPoints(p0, p1, p2);
}

void GPUBezierFilter::setPoints(gpu_point_t p0, gpu_point_t p1, gpu_point_t p2){
    std::vector<gpu_point_t>    seg_points;
    calSegmentPoints(p0, p1, p2, seg_points);
    GPULineFilter::setPoints(&seg_points[0], m_segment+1);
}

void GPUBezierFilter::setPoints(gpu_point_t* points, int n){
    std::vector<gpu_point_t>    bezier_points;
    
    if (n<=3) {
        std::vector<gpu_point_t> segment;
        calSegmentPoints(points[0], points[1], points[2], segment);
        bezier_points.insert(bezier_points.end(), segment.begin(), segment.end());
    }
    else{
        std::vector<gpu_point_t> segment;
        gpu_point_t p2 = {(points[1].x+points[2].x)/2, (points[1].y+points[2].y)/2};
        calSegmentPoints(points[0], points[1], p2, segment);
        bezier_points.insert(bezier_points.end(), segment.begin(), segment.end());
        
        for (int i=1; i<n-3; i++) {
            segment.clear();
            gpu_point_t p0 = {(points[i].x+points[i+1].x)/2, (points[i].y+points[i+1].y)/2};
            gpu_point_t p1 = points[i+1];
            gpu_point_t p2 = {(points[i+1].x+points[i+2].x)/2, (points[i+1].y+points[i+2].y)/2};
            calSegmentPoints(p0, p1, p2, segment);
            bezier_points.insert(bezier_points.end(), segment.begin(), segment.end());
        }
        
        segment.clear();
        gpu_point_t p0 = {(points[n-3].x+points[n-2].x)/2, (points[n-3].y+points[n-2].y)/2};
        calSegmentPoints(p0, points[n-2], points[n-1], segment);
        bezier_points.insert(bezier_points.end(), segment.begin(), segment.end());
    }
    
    m_segment = (uint32_t)(bezier_points.size()-1);
    GPULineFilter::setPoints(&bezier_points[0], m_segment);
}

void GPUBezierFilter::setColors(gpu_colorf_t c0, gpu_colorf_t c1){
    std::vector<gpu_colorf_t> colors;
    colors.resize(m_segment+1);
    
    float rstep = (c1.r-c0.r)/m_segment;
    float gstep = (c1.g-c0.g)/m_segment;
    float bstep = (c1.b-c0.b)/m_segment;
    float astep = (c1.a-c0.a)/m_segment;
    for (int i=0; i<=m_segment; i++) {
        colors[i].r = c0.r + rstep*i;
        colors[i].g = c0.g + gstep*i;
        colors[i].b = c0.b + bstep*i;
        colors[i].a = c0.a + astep*i;
    }
    
    GPULineFilter::setColors(&colors[0], m_segment+1);
}

void GPUBezierFilter::calSegmentPoints(gpu_point_t p0, gpu_point_t p1, gpu_point_t p2, std::vector<gpu_point_t>& segments){
    m_vertex_points[0] = p0;
    m_vertex_points[1] = p1;
    m_vertex_points[2] = p2;
    
    float d01 = sqrt(pow(p1.x-p0.x, 2)+pow(p1.y-p0.y, 2));
    float d12 = sqrt(pow(p2.x-p1.x, 2)+pow(p2.y-p1.y, 2));
    
    float t = d01/(d01+d12);
    m_control_point.x = (p1.x - (pow(1-t, 2)*p0.x + pow(t,2)*p2.x)) / (2*t*(1-t));
    m_control_point.y = (p1.y - (pow(1-t, 2)*p0.y + pow(t,2)*p2.y)) / (2*t*(1-t));
    
    // 每10个像素一条线段
    m_segment = ceil((d01+d12)/10);
    
    segments.clear();
    float step = 1.0/m_segment;
    for (int i=0; i<=m_segment; i++) {
        gpu_point_t p;
        float t = i*step;
        p.x = pow(1-t, 2)*p0.x + 2*t*(1-t)*m_control_point.x + pow(t, 2)*p2.x;
        p.y = pow(1-t, 2)*p0.y + 2*t*(1-t)*m_control_point.y + pow(t, 2)*p2.y;
        segments.push_back(p);
    }
}

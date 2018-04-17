/**
 * file :	GPUOutputFilter.cpp
 * author :	Rex
 * email : rex@labjk.com
 * create :	2017-03-25 16:11
 * func : 
 * history:
 */

#include "GPUOutputFilter.h"

GPUOutputFilter::GPUOutputFilter(gpu_pixel_format_t format){
    m_input = &m_zoom_filter;
    switch (format) {
        case GPU_BGRA:
            m_zoom_filter.setOutputFormat(GPU_BGRA);
        case GPU_RGBA:
            m_output = &m_zoom_filter;
            m_zoom_filter.addTarget(&m_raw_output);
            break;
        case GPU_I420:
            m_420_filter = new GPUToYUV420Filter();
        case GPU_NV12:
            m_420_filter = new GPUToNV12Filter();
        case GPU_NV21:
            m_420_filter = new GPUToNV21Filter();
            m_zoom_filter.addTarget(&m_yuv_filter);
            m_yuv_filter.addTarget(m_420_filter);
            m_420_filter->addTarget(&m_raw_output);
            break;
        default:
            err_log("Not supported output pixel format!");
            break;
    }
}

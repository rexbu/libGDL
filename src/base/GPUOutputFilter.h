/**
 * file :	GPUOutputFilter.h
 * author :	Rex
 * email : rex@labjk.com
 * create :	2017-03-25 16:11
 * func :   输出group，支持bgra、rgba、nv12、nv21、i420等格式输出
 * history:
 */

#ifndef	__GPUOUTPUTFILTER_H_
#define	__GPUOUTPUTFILTER_H_

#include "GPUGroupFilter.h"
#include "GPUZoomFilter.h"
#include "GPUYUVFilter.h"
#include "GPUYUV420Filter.h"
#include "GPURawOutput.h"

class GPUOutputFilter: public GPUGroupFilter{
public:
    GPUOutputFilter(gpu_pixel_format_t format);
    
    inline unsigned char* getBuffer(unsigned char* buffer = NULL, uint32_t size = 0){
        return m_raw_output.getBuffer(buffer, size);
    }
protected:
    GPUZoomFilter       m_zoom_filter;
    GPURGBToYUVFilter   m_yuv_filter;
    GPUFilter*          m_420_filter;
    GPURawOutput        m_raw_output;
};

#endif

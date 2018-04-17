/**
 * file :	GPUYUV420Filter.h
 * author :	Rex
 * email : rex@labjk.com
 * create :	2016-07-22 18:37
 * func : 
 * history:
 */

#ifndef	__GPUYUV420FILTER_H_
#define	__GPUYUV420FILTER_H_

#include "GPUFilter.h"
#include "GPURawOutput.h"

// YUV420分为I420和YV12， I420格式：yyyyyyyyuuvv， YV12格式：yyyyyyyyvvuu。此处为I420
class GPUToYUV420Filter:public GPUFilter{
public:
	GPUToYUV420Filter();
    GPUToYUV420Filter(const char* fragment);

protected:
	virtual gpu_size_t sizeOfFBO();
    virtual void setFrameSize(uint32_t width, uint32_t height);
};
// NV21格式：yyyyyyyyvuvu
class GPUToNV21Filter:public GPUToYUV420Filter{
public:
    GPUToNV21Filter();
    virtual void setFrameSize(uint32_t width, uint32_t height);
};
// NV12格式：yyyyyyyyuvuv
class GPUToNV12Filter:public GPUToYUV420Filter{
public:
    GPUToNV12Filter();
    virtual void setFrameSize(uint32_t width, uint32_t height);
};

class GPUYUV420PreciseOutput: public GPUInput{
public:
    GPUYUV420PreciseOutput();  // 默认直到获取buffer时候才从gpu中读取
    ~GPUYUV420PreciseOutput();
    // 外部传入buffer
    void setBuffer(unsigned char* buffer, uint32_t size);
    // 从buffer中获取
    unsigned char * getBuffer(unsigned char* buffer = NULL, uint32_t size = 0);
    int getSize(){
        return m_size;
    }
    
    virtual void newFrame();
    virtual void setInputFrameBuffer(GPUFrameBuffer* buffer, int location=0);
    
protected:
    GPUFrameBuffer* m_inputbuffer;
    GPUFilter       m_u_filter;
    GPUFilter       m_v_filter;
    
    unsigned char*  m_bytebuffer;
    int             m_size;
    bool            m_frame_get;
};
#endif

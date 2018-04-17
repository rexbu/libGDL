/**
 * file :	GPURawOutput.h
 * author :	Rex
 * email : rex@labjk.com
 * create :	2016-06-01 14:41
 * func : 
 * history:
 */

#ifndef	__GPURAWOUTPUT_H_
#define	__GPURAWOUTPUT_H_

#include "GPUContext.h"
#include "GPUFrameBuffer.h"
#include "GPUInput.h"

class GPURawOutput:public GPUInput{
public:
    GPURawOutput(bool direct = false);  // 默认直到获取buffer时候才从gpu中读取
    ~GPURawOutput();
    // 外部传入buffer
    void setBuffer(unsigned char* buffer, uint32_t size);
    // 从buffer中获取
    unsigned char * getBuffer(unsigned char* buffer = NULL, uint32_t size = 0);
    int getTexture();
    int getSize(){
        return m_size;
    }
    
    virtual void newFrame();
    virtual void setInputFrameBuffer(GPUFrameBuffer* buffer, int location=0);
    
protected:
    GPUFrameBuffer* m_firstbuffer;

    unsigned char*  m_bytebuffer;
    int             m_size;
    bool            m_direct;
    bool            m_frame_get;
};

#endif

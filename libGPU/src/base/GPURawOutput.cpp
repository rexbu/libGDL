/**
 * file :	GPURawOutput.cpp
 * author :	Rex
 * email : rex@labjk.com
 * create :	2016-06-01 14:41
 * func : 
 * history:
 */

#include "GPURawOutput.h"

GPURawOutput::GPURawOutput(bool direct){
    m_size = 0;
    m_bytebuffer = NULL;
    m_direct = direct;
    m_frame_get = true;
    m_firstbuffer = NULL;
}

GPURawOutput::~GPURawOutput(){
    if (m_bytebuffer!=NULL) {
        delete []m_bytebuffer;
        m_bytebuffer = NULL;
        m_size = 0;
    }
}

void GPURawOutput::newFrame(){
    if (m_direct)
    {
        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        m_firstbuffer->getPixels(m_bytebuffer);
        glFinish();
        m_firstbuffer->unlock();
        // 标记已经读取
        m_frame_get = true;
    }
}

unsigned char* GPURawOutput::getBuffer(unsigned char* buffer, uint32_t size){
    if (m_direct)
    {
        return m_bytebuffer;
    }
    else{
        if (buffer==NULL || size<m_size)
        {
            if (m_firstbuffer == NULL) {
                return NULL;
            }
            glPixelStorei(GL_PACK_ALIGNMENT, 1);
            m_firstbuffer->getPixels(m_bytebuffer);
            glFinish();
            m_firstbuffer->unlock();
            // 标记已经读取
            m_frame_get = true;
            return m_bytebuffer;
        }
        else{
            glPixelStorei(GL_PACK_ALIGNMENT, 1);
            m_firstbuffer->getPixels(buffer);
            glFinish();
            m_firstbuffer->unlock();
            // 标记已经读取
            m_frame_get = true;
            return buffer;
        }
    }
}

int GPURawOutput::getTexture(){
    int texture = m_firstbuffer->m_texture;
    if (!m_direct)
    {
        m_firstbuffer->unlock();
        m_frame_get = true;
    }
    return texture;
}

void GPURawOutput::setBuffer(unsigned char* buffer, uint32_t size){
    m_bytebuffer = buffer;
    m_size = size;
}

void GPURawOutput::setInputFrameBuffer(GPUFrameBuffer* buffer, int location){
    // 如果上一帧还没有被读取，没有释放需要释放
    if (!m_frame_get) {
        m_firstbuffer->unlock();
    }
    
    m_frame_get = false;
    m_firstbuffer = buffer;
    m_firstbuffer->lock();
    if (m_bytebuffer==NULL) {
        m_size = buffer->m_width*buffer->m_height*4;
        m_bytebuffer = new unsigned char[m_size];
        err_log("RawOutput alloc buffer[%d]", m_size);
    }
    else if(m_size < buffer->m_width*buffer->m_height*4){
        err_log("buf small, delete buf");
        delete []m_bytebuffer;
        m_size = buffer->m_width*buffer->m_height*4;
        m_bytebuffer = new unsigned char[m_size];
        err_log("RawOutput realloc buffer[%d]", m_size);
    }
    memset(m_bytebuffer, 0, m_size);
}

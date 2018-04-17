/**
 * file :	GPUGroupFilter.cpp
 * author :	Rex
 * email : rex@labjk.com
 * create :	2016-08-06 21:57
 * func : 
 * history:
 */

#include "GPUGroupFilter.h"

GPUGroupFilter::GPUGroupFilter(const char* name):
GPUFilter(false, name==NULL?"GroupFilter":name){
    m_input = NULL;
    m_output = NULL;
}

void GPUGroupFilter::newFrame(){
#if DEBUG_FILTER_NAME
    err_log("filter name: %s input:%p", m_filter_name.c_str(), m_input);
#endif
    
    GPUCheckGlError(m_filter_name.c_str());
    if (!m_disable && m_input!=NULL) {
        // 在设置时候有可能input还没有生成
//        m_input->setOutputRotation(m_rotation);
//        m_input->setOutputSize(m_out_width, m_out_height);
        
        m_input->newFrame();
        if (m_output->m_outbuffer==NULL) {
            err_log("%s newFrame Failed!", m_filter_name.c_str());
            return;
        }
        m_outbuffer = m_output->m_outbuffer;
    }
    else{
        m_outbuffer = m_input_buffers[0];
    }
    
    GPUOutput::informTargets();
}

void GPUGroupFilter::setInputFrameBuffer(GPUFrameBuffer* buffer, int location){
    // GPUGroupFilter的inputs为1，用m_input_buffers[0]存储第一个framebuffer，用于bypass时的传递
    if (location==0 && buffer!=NULL) {
        m_input_buffers[location] = buffer;
        if (m_frame_width!=buffer->m_width || m_frame_height!=buffer->m_height) {
            setFrameSize(buffer->m_width, buffer->m_height);
        }
    }
    
    if (!m_disable && m_input!=NULL) {
        m_input->setInputFrameBuffer(buffer, location);
    }
}

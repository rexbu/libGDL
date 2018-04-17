/**
 * file :	GPUTwoPassFilter.cpp
 * author :	Rex
 * email : rex@labjk.com
 * create :	2016-05-05 19:49
 * func : 
 * history:
 */

#include "GPUTwoPassFilter.h"

GPUTwoPassFilter::GPUTwoPassFilter():GPUFilter(false){
    m_first_filter = NULL;
    m_second_filter = NULL;
}

GPUTwoPassFilter::GPUTwoPassFilter(const char* first_fragment, const char* second_fragment):GPUFilter(false){
    m_first_filter = new GPUFilter(first_fragment);
    m_second_filter = new GPUFilter(second_fragment);
}

GPUTwoPassFilter::GPUTwoPassFilter(const char* first_vertext, const char* first_fragment, const char* second_vertext, const char* second_fragment):GPUFilter(false){
    m_first_filter = new GPUFilter(first_vertext, first_fragment);
    m_second_filter = new GPUFilter(second_vertext, second_fragment);
}

void GPUTwoPassFilter::init_filter(){
    
}
void GPUTwoPassFilter::setInputFrameBuffer(GPUFrameBuffer* buffer, int location){
    m_input_buffers[0] = buffer;
    m_first_filter->setInputFrameBuffer(buffer, location);
    
    if (m_frame_width!=buffer->m_width || m_frame_height!=buffer->m_height) {
        setFrameSize(buffer->m_width, buffer->m_height);
    }
}

void GPUTwoPassFilter::render(){
#if DEBUG_FILTER_NAME
    err_log("filter name: %s", m_filter_name.c_str());
#endif
    
    m_first_filter->newFrame();
    m_second_filter->setInputFrameBuffer(m_first_filter->m_outbuffer);
    m_second_filter->newFrame();
    m_outbuffer = m_second_filter->m_outbuffer;
    // render后的回调
    if (m_complete!=NULL) {
        m_complete(this, m_para);
    }
}

void GPUTwoPassFilter::changeShader(const char* first_fragment, const char* second_fragment){
    m_first_filter->changeShader(first_fragment);
    m_second_filter->changeShader(second_fragment);
}

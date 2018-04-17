/**
 * file :	GPUPaintFilter.cpp
 * author :	Rex
 * create :	2017-05-23 13:25
 * func : 
 * history:
 */

#include "GPUPaintFilter.h"

GPUPaintFilter::GPUPaintFilter(int w, int h):
GPUFilter(false){
    m_out_width = w;
    m_out_height = h;
    m_inputs = 0;
    m_line_width = 1;
    
	memset(&m_c0, 0, sizeof(gpu_colorf_t));
	memset(&m_c1, 0, sizeof(gpu_colorf_t));
	m_c0.a = 1.0;
	m_c1.a = 1.0;
}

GPUPaintFilter::~GPUPaintFilter(){
    clear();
}

void GPUPaintFilter::clear(){
    for (int i=0; i<m_bezier_filters.size(); i++) {
        delete m_bezier_filters[i];
    }
    
    m_bezier_filters.clear();
}

void GPUPaintFilter::addPoints(gpu_point_t* points, int n){
    GPUBezierFilter* bezier = new GPUBezierFilter(m_out_width, m_out_height);
    bezier->setPoints(points, n);
    m_bezier_filters.push_back(bezier);
}

void GPUPaintFilter::newFrame(){
    if (ready())
    {
        if (m_disable || m_bezier_filters.size()<=0) {
            m_outbuffer = m_input_buffers[0];
            unlockInputFrameBuffers();
            informTargets();
        }
        else{
            GPUContext* context = GPUContext::shareInstance();
            context->glContextLock();   // 加锁，防止此时设置参数
            //context->setActiveProgram(m_program);
            
            if (m_special_outbuffer!=NULL && m_bezier_filters.size()>0) {
                m_bezier_filters[0]->setOutputFrameBuffer(m_special_outbuffer);
            }
            
            // 上一次的outputbuffer或者本次渲染的fbo
            for (int i=0; i<m_bezier_filters.size(); i++) {
                if (i>0) {
                    m_bezier_filters[i]->setOutputFrameBuffer(m_bezier_filters[i-1]->m_outbuffer);
                }
                m_bezier_filters[i]->activeOutFrameBuffer();
                m_bezier_filters[i]->setColors(m_c0, m_c1);
                m_bezier_filters[i]->setLineWidth(m_line_width);
                m_bezier_filters[i]->render();
                
                m_outbuffer = m_bezier_filters[i]->m_outbuffer;
            }
            
            context->glContextUnlock();
            m_special_outbuffer = NULL;
            
            // render后的回调
            if (m_complete!=NULL) {
                m_complete(this, m_para);
            }
            
            unlockInputFrameBuffers();
            informTargets();
        }
    }
}

void GPUPaintFilter::setColors(gpu_colorf_t c0, gpu_colorf_t c1){
    m_c0 = c0;
    m_c1 = c1;
}

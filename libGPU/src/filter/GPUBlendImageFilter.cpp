/**
 * file :	GPUBlendImageFilter.cpp
 * author :	Rex
 * create :	2017-04-14 16:27
 * func : 
 * history:
 */

#include "GPUBlendImageFilter.h"

GPUBlendImageFilter::GPUBlendImageFilter(int image_num):
GPUBlendFilter(image_num+1)
{
    m_images.resize(image_num);
    memset(&m_images[0], 0, sizeof(GPUPicture*)*image_num);
    disable();
}

void GPUBlendImageFilter::setBlendImageRect(GPUPicture* pic, gpu_rect_t rect, int index){
    gpu_point_t points[4];
    points[0].x = rect.pointer.x;
    points[0].y = rect.pointer.y;
    points[1].x = rect.pointer.x + rect.size.width;
    points[1].y = rect.pointer.y;
    points[2].x = rect.pointer.x;
    points[2].y = rect.pointer.y + rect.size.height;
    points[3].x = rect.pointer.x + rect.size.width;
    points[3].y = rect.pointer.y + rect.size.height;
    setBlendImage(pic, points, index);
}

void GPUBlendImageFilter::setBlendImage(GPUPicture* pic, gpu_point_t points[4], int index){
    if (index>m_images.size()-1) {
        return;
    }
    // 取消
    if(pic==NULL){
        if (m_images[index]!=NULL) {
            m_images[index]->removeAllTargets();
            delete m_images[index];
        }
        disable();
    }
    
    if (m_images[index]!=NULL) {
        m_images[index]->removeAllTargets();
        delete m_images[index];
    }
    
    m_images[index] = pic;
    setDrawRect(points, index+1);
    
    GPUContext* context = GPUContext::shareInstance();
    context->glContextLock();
    pic->addTarget(this, index+1);
    context->glContextUnlock();
    enable();
}

void GPUBlendImageFilter::setInputFrameBuffer(GPUFrameBuffer *buffer, int location){
    if (location==0 && !m_disable) {
        for (int i=0; i<m_images.size(); i++) {
            if (m_images[i]==NULL) {
                err_log("Error: Blend Image %d null! ", i);
                break;
            }
            m_images[i]->processImage();
        }
    }
    
    GPUBlendFilter::setInputFrameBuffer(buffer, location);
}

/**
 * file :	GPUView.cpp
 * author :	Rex
 * create :	2016-04-29 01:19
 * func : 
 * history:
 */

#include <jni.h>
#include "GPUView.h"

GPUView::GPUView(int width, int height):
GPUFilter(true, "GPUView")
{
    m_rotation = GPUNoRotation;
    m_fill_mode = GPUFillModePreserveAspectRatioAndFill;
    m_eglsurface = EGL_NO_SURFACE;
    setOutputSize(width, height);
}

GPUView::GPUView(ANativeWindow * surface):
GPUFilter(true, "GPUView")
{
    m_rotation = GPUNoRotation;
    m_fill_mode = GPUFillModePreserveAspectRatioAndFill;
    m_eglsurface = EGL_NO_SURFACE;
    setNativeWindow(surface);
    calAspectRatio();
}
GPUView::~GPUView(){
    if (m_eglsurface != EGL_NO_SURFACE) {
        GPUContext* context = GPUContext::shareInstance();
        if(!eglMakeCurrent(context->m_gpu_context->egldisplay, m_eglsurface, m_eglsurface, context->m_gpu_context->eglcontext)) {
            err_log("Set Windows Context Error.");
        }
        eglDestroySurface(context->m_gpu_context->egldisplay, m_eglsurface);
        m_eglsurface = EGL_NO_SURFACE;
    }
}

void GPUView::newFrame(){
#if DEBUG_FILTER_NAME
    err_log("filter name: %s texture: %d", m_filter_name.c_str(), m_firstbuffer->m_texture);
#endif

    GPUContext* context = GPUContext::shareInstance();
    context->glContextLock();   // 加锁，防止此时设置参数
    context->setActiveProgram(m_program);
    
    if(!eglMakeCurrent(context->m_gpu_context->egldisplay, m_eglsurface, m_eglsurface, context->m_gpu_context->eglcontext)) {
        err_log("Set Windows Context Error.");
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, (int)m_out_width, (int)m_out_height);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    
    m_firstbuffer->activeTexture(GL_TEXTURE2);
    glUniform1i(m_input_texture, 2);
    
    GPUVertexBuffer* vertex_buffer = GPUVertexBufferCache::shareInstance()->getVertexBuffer();
    vertex_buffer->activeBuffer(m_position, &m_vertices[0]);
    GPUVertexBuffer* coor_buffer = GPUVertexBufferCache::shareInstance()->getVertexBuffer();
    coor_buffer->activeBuffer(m_input_coordinate, GPUFilter::coordinatesRotation(m_rotation));
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    context->swapBuffer(m_eglsurface);
    context->glContextUnlock();
    vertex_buffer->unLock();
    coor_buffer->unLock();

    m_firstbuffer->unlock();

    // 切换回PBufferSurface，防止之后的一些操作没有重新设置glcontext引起的错误
    GPUContext::shareInstance()->makeCurrent();
}

void GPUView::setNativeWindow(ANativeWindow * surface){
    m_surface = surface;

    GPUContext* context = GPUContext::shareInstance();
    context->glContextLock();
    context->makeCurrent();

    EGLint config_attribs[] = {
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_NONE
    };

    int num_configs = 0;
    EGLConfig   eglConfig;
    if(!eglChooseConfig(context->m_gpu_context->egldisplay, config_attribs, &eglConfig, 1, &num_configs))
    {
        err_log("eglChooseConfig Error!");
        return ;
    }

    if (m_eglsurface != EGL_NO_SURFACE) {
        eglDestroySurface(context->m_gpu_context->egldisplay, m_eglsurface);
    }
    
    m_eglsurface = eglCreateWindowSurface(context->m_gpu_context->egldisplay, eglConfig, m_surface, NULL);
    if(EGL_NO_SURFACE == m_eglsurface) {
        err_log("eglCreateWindowSurface Error!");
        return;
    }

    if(!eglQuerySurface(context->m_gpu_context->egldisplay, m_eglsurface, EGL_WIDTH, (EGLint*)&m_out_width) ||
        !eglQuerySurface(context->m_gpu_context->egldisplay, m_eglsurface, EGL_HEIGHT, (EGLint*)&m_out_height)) {
        err_log("eglQuerySurface Error!");
        return ;
    }
    info_log("WindowSurface width:%d height:%d", m_out_width, m_out_height);

    context->glContextUnlock();
}
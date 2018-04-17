/**
 * file :	GPUContext.cpp
 * author :	Rex
 * create :	2016-07-18 19:15
 * func : 
 * history:
 */

#include "GPUContext.h"

void GPUContext::createContext(){
    m_gpu_context = new _gpu_context_t;
    memset(m_gpu_context, 0, sizeof(_gpu_context_t));
	m_gpu_context->egldisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if(EGL_NO_DISPLAY == m_gpu_context->egldisplay) {
        err_log("eglGetDisplay Error!");
        return;
    }
    
    GLint majorVersion;
    GLint minorVersion;
    if(!eglInitialize(m_gpu_context->egldisplay, &majorVersion, &minorVersion)) {
        err_log("eglInitialize Error!");
        return;
    }
    info_log("GL Version minor:%d major:%d", minorVersion, majorVersion);
    
    // 如果创建WindowSurface使用EGL_WINDOW_BIT，PBufferSurface使用EGL_PBUFFER_BIT
    EGLint config_attribs[] = {
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        EGL_NONE
    };

    int num_configs = 0;
    EGLConfig   eglConfig;
    if(!eglChooseConfig(m_gpu_context->egldisplay, config_attribs, &eglConfig, 1, &num_configs))
    {
        err_log("eglChooseConfig Error!");
        return ;
    }

    EGLint context_attrib[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };
    m_gpu_context->eglcontext = eglCreateContext(m_gpu_context->egldisplay, eglConfig, EGL_NO_CONTEXT, context_attrib);
    if(EGL_NO_CONTEXT == m_gpu_context->eglcontext) {
        err_log("eglCreateContext Error!");
        return ;
    }

    int attribListPbuffer[] = {  
        EGL_WIDTH, m_surfacewidth,  
        EGL_HEIGHT, m_surfaceheight,  
        EGL_NONE  
    };

    m_gpu_context->eglsurface = eglCreatePbufferSurface(m_gpu_context->egldisplay, eglConfig, attribListPbuffer);
    if(EGL_NO_SURFACE == m_gpu_context->eglsurface) {
        err_log("eglCreatePbufferSurface Error!");
        return;
    }
    if(!eglQuerySurface(m_gpu_context->egldisplay, m_gpu_context->eglsurface, EGL_WIDTH, &m_surfacewidth) ||
       !eglQuerySurface(m_gpu_context->egldisplay, m_gpu_context->eglsurface, EGL_HEIGHT, &m_surfaceheight)) {
        err_log("eglQuerySurface Error!");
        return ;
    }
    info_log("Create Surface width:%d height:%d", m_surfacewidth, m_surfaceheight);
}
// 作为当前的glContext
void GPUContext::makeCurrent(){
    if (m_has_context){
        if(!eglMakeCurrent(m_gpu_context->egldisplay, m_gpu_context->eglsurface,m_gpu_context->eglsurface, m_gpu_context->eglcontext)) {
            err_log("Set Current Context Error.");
        }
    }
}
void GPUContext::swapBuffer(void* surface){
    eglSwapBuffers(m_gpu_context->egldisplay, (EGLSurface)surface);
}

GPUContext::~GPUContext(){
	if (m_gpu_context!=NULL && m_gpu_context->egldisplay != EGL_NO_DISPLAY) {
        if (m_gpu_context->eglcontext != EGL_NO_CONTEXT) {
            eglDestroyContext(m_gpu_context->egldisplay, m_gpu_context->eglcontext);
        }
        
        if (m_gpu_context->eglsurface != EGL_NO_SURFACE) {
            eglDestroySurface(m_gpu_context->egldisplay, m_gpu_context->eglsurface);
        }

        eglMakeCurrent(m_gpu_context->egldisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (!eglTerminate(m_gpu_context->egldisplay)) {
            err_log("Free egldisplay error!");
        }
    }
    
    if(m_gpu_context!=NULL){
        delete m_gpu_context;
        m_gpu_context = NULL;
    }
}
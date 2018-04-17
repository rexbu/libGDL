/**
 * file :	gpu_view.h
 * author :	Rex
 * email : rex@labjk.com
 * create :	2016-04-27 19:13
 * func : 
 * history:
 */

#ifndef	__GPU_VIEW_H_
#define	__GPU_VIEW_H_
#ifdef __ANDROID__
#include <EGL/egl.h>
#include <android/native_window_jni.h>
#endif
#include "GPUFilter.h"

class GPUView: public GPUFilter{
// class GPUView: public GPUFilter{
public:
    GPUView(int width, int height);
#ifdef __ANDROID__
    GPUView(ANativeWindow * surface);
    void setNativeWindow(ANativeWindow * surface);
#endif
    ~GPUView();

    virtual void newFrame();
    
protected:
    GLuint  m_displaybuffer;
    GLuint  m_displayrender;

#ifdef __ANDROID__
    void createWindowContext();

    EGLSurface		m_eglsurface;
    ANativeWindow*	m_surface;
#endif
};

#endif

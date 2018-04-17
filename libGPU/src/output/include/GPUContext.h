/**
 * file :	GPUContext.h
 * author :	Rex
 * email : rex@labjk.com
 * create :	2016-04-27 19:29
 * func : 
 * history:
 */

#ifndef	__GL_CONTEXT_H_
#define	__GL_CONTEXT_H_

#ifdef __ANDROID__
#include <jni.h>
#include <GLES/gl.h>
#include <GLES/glext.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <EGL/egl.h>
#include <android/log.h>

typedef struct _gpu_context_t{
    EGLDisplay			egldisplay;
	EGLSurface			eglsurface;
	EGLContext			eglcontext;
}_gpu_context_t;

#elif __IOS__
#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>
struct _gpu_context_t;

#else
#import <OpenGL/OpenGL.h>
#import <OpenGL/gl.h>
#endif

#include "bs.h"
#include "GL.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <iostream>
#include <vector>

#define DEBUG_FILTER_NAME   0     // 打开filter链条打印debug

#define	SHADER_PREFIX	"#extension GL_OES_EGL_image_external : require\nprecision mediump float;"
#define	STRINGIZE(x)	#x
#define	SHADER_OESSTRING(text)	SHADER_PREFIX STRINGIZE(text)
#define	SHADER_STRING(text)		"precision mediump float;" STRINGIZE(text)
#define SHADER30_STRING(text)   "#version 300 es\nprecision highp float;precision highp int;" STRINGIZE(text)

#define GPURotationSwapsWidthAndHeight(rotation) ((rotation) == GPURotateLeft || (rotation) == GPURotateRight || (rotation) == GPURotateRightFlipVertical || (rotation) == GPURotateRightFlipHorizontal)
void GPUCheckGlError(const char* op, bool log=true, bool lock=true);

typedef struct gpu_task_t{
    void* (*task)(void* para);
    void* para;
}gpu_task_t;

class GPUProgram;
class GPUFrameBuffer;

class GPUContext{
public:
    static GPUContext* shareInstance();
    static void destroyInstance();
    
    virtual void createContext();
    
    // 作为当前glContext
    virtual void makeCurrent();
    virtual void swapBuffer(void* surface=NULL);
    
    // texture能存储的最大尺寸
    virtual int maximumTextureSize();
    // fragment shader最大纹理数量
    static int maxFragmentTextureCount();
    
    void setActiveProgram(GPUProgram* program);
    
    gpu_size_t sizeFitsTextureMaxSize(gpu_size_t size);
    
    // glContext锁，主要用于render过程中不能设置参数
    int glContextLock();
    int glContextUnlock();
    
    // 安卓层可能已经设置好glcontext，
    static void setContextEnable(bool has){
        m_has_context = has;
    }
    static int maxRenderBufferSize(){
        int size;
        glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &size);
        return size;
    }
    /// 一些参数打印
    static void printParams();
    
#ifdef __IOS__
    void* coreVideoTextureCache();
#endif
#ifdef __ANDROID__
    int 				m_surfacewidth;
	int 				m_surfaceheight;
#endif
    _gpu_context_t*     m_gpu_context;

protected:
    GPUContext();
    virtual ~GPUContext();
    static GPUContext*  m_instance;
    
    // 当前program
    GPUProgram*         m_current_program;
    // glContext render及参数设置锁
    pthread_mutex_t     m_lock;
    static bool         m_has_context;

// 用于执行需要和new frame异步的任务，目前是更换滤镜
public:
    void pushAsyncTask(void* (*task)(void* para), void* para){
        gpu_task_t t = {task, para};
        m_tasks.push_back(t);
    }
    void runAsyncTasks(){
        for (int i=0; i<m_tasks.size(); i++) {
            m_tasks[i].task(m_tasks[i].para);
        }
        m_tasks.clear();
    }    
    // 用于执行需要和new frame异步的任务，目前是更换滤镜
    std::vector<gpu_task_t> m_tasks;
};

#endif

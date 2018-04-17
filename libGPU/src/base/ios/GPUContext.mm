/**
 * file :	GPUContext.cpp
 * author :	Rex
 * email : rex@labjk.com
 * create :	2016-07-06 16:33
 * func : 
 * history:
 */

#include "GPUContext.h"
#include <Foundation/Foundation.h>
#include <AVFoundation/AVFoundation.h>
#import <UIKit/UIKit.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#import <OpenGLES/EAGL.h>

EAGLContext* g_share_context = NULL;

typedef struct _gpu_context_t{
    EAGLContext*                context;
    EAGLSharegroup*             sharegroup;
    CVOpenGLESTextureCacheRef   texture_cache;
}_gpu_context_t;

GPUContext::~GPUContext()
{
    pthread_mutex_destroy(&m_lock);
    if (m_gpu_context!=NULL) {
        CFRelease(m_gpu_context->texture_cache);
        delete m_gpu_context;
        m_gpu_context = NULL;
    }
}

void GPUContext::createContext(){
    m_gpu_context = new _gpu_context_t;
    memset(m_gpu_context, 0, sizeof(_gpu_context_t));
    
    if (g_share_context==NULL) {
        m_gpu_context->context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3 sharegroup:m_gpu_context->sharegroup];
        //NSAssert(m_context != NULL, @"Unable to create an OpenGL ES 2.0 context. The GPUImage framework requires OpenGL ES 2.0 support to work.");
        assert(m_gpu_context->context!=NULL);
    }
    else{
        m_gpu_context->context = g_share_context;
    }
    
    [EAGLContext setCurrentContext:m_gpu_context->context];
    printParams();
    // Set up a few global settings for the image processing pipeline
    glDisable(GL_DEPTH_TEST);
    // glEnable(GL_DEPTH_TEST);
}

// 作为当前的glContext
void GPUContext::makeCurrent(){
    if (m_has_context) {
        if ([EAGLContext currentContext] != m_gpu_context->context) {
            [EAGLContext setCurrentContext:m_gpu_context->context];
        }
    }
}

void* GPUContext::coreVideoTextureCache(){
    if (m_gpu_context->texture_cache == NULL)
    {
#if defined(__IPHONE_6_0)
        CVReturn err = CVOpenGLESTextureCacheCreate(kCFAllocatorDefault, NULL, m_gpu_context->context, NULL, &m_gpu_context->texture_cache);
#else
        CVReturn err = CVOpenGLESTextureCacheCreate(kCFAllocatorDefault, NULL, (__bridge void *)[self context], NULL, &_coreVideoTextureCache);
#endif
        
        if (err)
        {
            err_log("Error at CVOpenGLESTextureCacheCreate %d", err);
            assert(false);
        }
    }
    
    return m_gpu_context->texture_cache;
}

void GPUContext::swapBuffer(void* surface)
{
    [m_gpu_context->context presentRenderbuffer:GL_RENDERBUFFER];
}

extern "C" EAGLContext* currentEAGLContext(){
    return GPUContext::shareInstance()->m_gpu_context->context;
}

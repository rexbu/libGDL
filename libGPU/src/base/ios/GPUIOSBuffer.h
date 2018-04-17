/**
 * file :	GPUIOSBuffer.h
 * author :	Rex
 * email : rex@labjk.com
 * create :	2016-07-08 16:08
 * func : 
 * history:
 */

#ifndef	__GPUIOSBUFFER_H_
#define	__GPUIOSBUFFER_H_

#import <Foundation/Foundation.h>
#import <QuartzCore/QuartzCore.h>
#import <CoreMedia/CoreMedia.h>
#include "GPUFrameBuffer.h"

class GPUIOSFrameBuffer: public GPUFrameBuffer{
public:
    GPUIOSFrameBuffer(gpu_size_t size, bool only_texture = true);
    GPUIOSFrameBuffer(int width, int height, bool only_texture = true);
    GPUIOSFrameBuffer(int width, int height, gpu_frame_option_t* option, bool only_texture = true);
    GPUIOSFrameBuffer(int width, int height, GLuint texture);
    
    virtual void generateFrameBuffer();
    CVPixelBufferRef getPixelBuffer(){
        return m_render_target;
    }
    
    ~GPUIOSFrameBuffer();
protected:
    
    CVPixelBufferRef m_render_target;
    CVOpenGLESTextureRef m_render_texture;
};

class GPUIOSBufferCache:public GPUBufferCache{
public:
    static GPUBufferCache* shareInstance();
protected:
    
    virtual GPUFrameBuffer* newFrameBuffer(int width, int height, gpu_frame_option_t* option, bool only_texture=true);
};
#endif

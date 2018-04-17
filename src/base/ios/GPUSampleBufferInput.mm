/**
 * file :	GPUSampleBufferInput.mm
 * author :	Rex
 * email : rex@labjk.com
 * create :	2017-03-25 20:41
 * func : 
 * history:
 */

#include "GPUSampleBufferInput.h"
#include "GPUContext.h"
#include "GPUIOSBuffer.h"

GPUSampleBufferInput::GPUSampleBufferInput(){
    m_format = 0;
}

void GPUSampleBufferInput::processSampleBuffer(CMSampleBufferRef sampleBuffer){
    CVImageBufferRef cameraFrame = CMSampleBufferGetImageBuffer(sampleBuffer);
    m_frame_width = (uint32_t) CVPixelBufferGetWidth(cameraFrame);
    m_frame_height = (uint32_t) CVPixelBufferGetHeight(cameraFrame);
    
    m_format = CVPixelBufferGetPixelFormatType(cameraFrame);
    
    GPUContext* context = GPUContext::shareInstance();
    context->makeCurrent();
    
    if (m_format == kCVPixelFormatType_420YpCbCr8BiPlanarFullRange || m_format==kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange) {
        CVOpenGLESTextureRef luminanceTextureRef = NULL;
        CVOpenGLESTextureRef chrominanceTextureRef = NULL;
        CVPixelBufferLockBaseAddress(cameraFrame, 0);
        
        context->glContextLock();
        CVReturn err;
        // Y-plane
        glActiveTexture(GL_TEXTURE4);
        err = CVOpenGLESTextureCacheCreateTextureFromImage(kCFAllocatorDefault, (CVOpenGLESTextureCacheRef)GPUContext::shareInstance()->coreVideoTextureCache(), cameraFrame, NULL, GL_TEXTURE_2D, GL_LUMINANCE, m_frame_width, m_frame_height, GL_LUMINANCE, GL_UNSIGNED_BYTE, 0, &luminanceTextureRef);
        
        if (err)
        {
            NSLog(@"Error at CVOpenGLESTextureCacheCreateTextureFromImage %d", err);
        }
        
        GLuint luminanceTexture = CVOpenGLESTextureGetName(luminanceTextureRef);
        glBindTexture(GL_TEXTURE_2D, luminanceTexture);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
        // UV-plane
        glActiveTexture(GL_TEXTURE5);
        err = CVOpenGLESTextureCacheCreateTextureFromImage(kCFAllocatorDefault, (CVOpenGLESTextureCacheRef)GPUContext::shareInstance()->coreVideoTextureCache(), cameraFrame, NULL, GL_TEXTURE_2D, GL_LUMINANCE_ALPHA, m_frame_width/2, m_frame_height/2, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, 1, &chrominanceTextureRef);
        
        if (err)
        {
            NSLog(@"Error at CVOpenGLESTextureCacheCreateTextureFromImage %d", err);
        }
        
        GLuint chrominanceTexture = CVOpenGLESTextureGetName(chrominanceTextureRef);
        glBindTexture(GL_TEXTURE_2D, chrominanceTexture);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        context->glContextUnlock();
        
        gpu_frame_option_t op = GPUFrameBuffer::defaultFrameOption();
        op.format = GL_LUMINANCE;
        GPUFrameBuffer luma_buffer(m_frame_width, m_frame_height, luminanceTexture);
        op.format = GL_LUMINANCE_ALPHA;
        GPUFrameBuffer chrom_buffer(m_frame_width, m_frame_height, chrominanceTexture);
        
        CVPixelBufferUnlockBaseAddress(cameraFrame, 0);
        if (m_input == NULL) {
            m_input = new GPUNV12ToRGBFilter();
            m_output = m_input;
        }
        
        setInputFrameBuffer(&luma_buffer, 0);
        setInputFrameBuffer(&chrom_buffer, 1);
        newFrame();
        
        CFRelease(luminanceTextureRef);
        CFRelease(chrominanceTextureRef);
    }
    else{
        CVPixelBufferLockBaseAddress(cameraFrame, 0);
        
        int bytesPerRow = (int) CVPixelBufferGetBytesPerRow(cameraFrame);
        context->glContextLock();
        GPUFrameBuffer* outbuffer = GPUIOSBufferCache::shareInstance()->getFrameBuffer(m_frame_width, m_frame_height, true);
        outbuffer->activeBuffer();
        glBindTexture(GL_TEXTURE_2D, outbuffer->m_texture);
        
        if (m_format==GPU_BGRA) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bytesPerRow / 4, m_frame_height, 0, GL_BGRA, GL_UNSIGNED_BYTE, CVPixelBufferGetBaseAddress(cameraFrame));
        }
        else{
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bytesPerRow / 4, m_frame_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, CVPixelBufferGetBaseAddress(cameraFrame));
        }
        context->glContextUnlock();
        CVPixelBufferUnlockBaseAddress(cameraFrame, 0);
        
        if (m_input==NULL) {
            m_input = new GPUFilter();
            m_output = m_input;
        }
        setInputFrameBuffer(outbuffer);
        newFrame();
    }
    
    // render后的回调
    if (m_complete!=NULL) {
        m_complete(this, m_para);
    }
}

GPUSampleBufferInput::~GPUSampleBufferInput(){
    if(m_input!=NULL){
        delete m_input;
    }
}

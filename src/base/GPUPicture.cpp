/**
 * file :	GPUPicture.cpp
 * author :	Rex
 * email : rex@labjk.com
 * create :	2016-08-10 17:06
 * func : 
 * history:
 */

#include "GPUPicture.h"

GPUPicture::GPUPicture(uint8_t * data, int width, int height){
    m_option = GPUFrameBuffer::defaultFrameOption();
    setPixel(data, width, height);
}
GPUPicture::GPUPicture(uint8_t * data, int width, int height, gpu_frame_option_t* format){
    m_option = *format;
    setPixel(data, width, height);
}
void GPUPicture::setPixel(uint8_t* pixels, uint32_t width, uint32_t height){
    m_pixels = pixels;
    m_image_size.width = width;
    m_image_size.height = height;
    
    GPUContext::shareInstance()->makeCurrent();
    // 如果图片被循环利用，使用GPUBufferCache获取会导致图片内容被改
    m_outbuffer = new GPUFrameBuffer(width, height, &m_option, true);
    m_outbuffer->disableReference();
    
    //m_outbuffer->activeTexture(GL_TEXTURE0);
    m_outbuffer->setPixels(pixels);
    // no need to use self.outputTextureOptions here since pictures need this texture formats and type
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (int)width, (int)height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
//    glBindTexture(GL_TEXTURE_2D, 0);
    
}

void GPUPicture::processImage(){
    for (int i=0; i<targetsCount(); i++) {
        GPUInput* input = m_targets[i];
        input->setInputFrameBuffer(m_outbuffer, m_location[i]);
        input->newFrame();
    }
}

GPUPicture::~GPUPicture(){
    m_outbuffer->release();
    delete m_outbuffer;
}

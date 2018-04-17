/**
 * file :	GPUTextureInput.cpp
 * author :	Rex
 * email : rex@labjk.com
 * create :	2016-12-08 15:39
 * func : 
 * history:
 */

#include "GPUTextureInput.h"

static const char* g_input_fragment_shader = SHADER_OESSTRING(
    varying vec2 textureCoordinate;
    uniform samplerExternalOES inputImageTexture;
    //uniform sampler2D inputImageTexture;
    void main()
    {
        gl_FragColor = texture2D(inputImageTexture, textureCoordinate);
    }
);

GPUTextureInput::GPUTextureInput()
:GPUFilter(g_input_fragment_shader)
{
    m_out_width = 0;
    m_out_height = 0;
    m_filter_name = "GPUTextureInput";
}

GPUTextureInput::GPUTextureInput(int width, int height, int texture_type)
:GPUFilter(texture_type==GPU_TEXTURE_OES ? 
    g_input_fragment_shader:GPUFilter::g_fragment_shader)
{
    m_filter_name = "GPUTextureInput";
    m_texture_type = texture_type;
    info_log("Visionin input texture type: %d", texture_type);
	setOutputSize(width, height);
/*
    // 竖屏前摄像头: kGPUImageRotateRight，交换宽高
    // 竖屏后摄像头：kGPUImageRotateRightFlipHorizontal，交换宽高
    // 横屏前摄像头：kGPUImageFlipVertical， 不交换宽高
    // 横屏后摄像头：kGPUImageFlipVertical， 不交换宽高
    m_rotation = GPUNoRotation;
    m_height = width;
    m_width = height;
    //m_outbuffer = new GPUFrameBuffer(m_width, m_height, texture);
*/
}

void GPUTextureInput::processTexture(){
    render();
    informTargets();
}

void GPUTextureInput::processTexture(GLuint texture){
	//setBufferType(fb);
    m_texture = texture;
    processTexture();
}

void GPUTextureInput::render(){
    GPUContext* context = GPUContext::shareInstance();
    context->glContextLock();   // 加锁，防止此时设置参数
    context->setActiveProgram(m_program);

    // active framebuffer
    m_outbuffer = GPUBufferCache::shareInstance()->getFrameBuffer(m_out_width, m_out_height, false);
    m_outbuffer->activeBuffer();
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE2);
    if (m_texture_type == GPU_TEXTURE_OES)
    {
#ifdef __ANDROID__
        glBindTexture(GL_TEXTURE_EXTERNAL_OES, m_texture);
#endif
    }
    else{
        glBindTexture(GL_TEXTURE_2D, m_texture);    
    }
    
    glUniform1i(m_input_texture, 2);

    GPUVertexBuffer* vertex_buffer = GPUVertexBufferCache::shareInstance()->getVertexBuffer();
    vertex_buffer->activeBuffer(m_position, GPUFilter::g_vertices);
    GPUVertexBuffer* coor_buffer = GPUVertexBufferCache::shareInstance()->getVertexBuffer();
    coor_buffer->activeBuffer(m_input_coordinate, GPUFilter::coordinatesRotation(m_rotation));

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    vertex_buffer->disableBuffer(m_position);
    coor_buffer->disableBuffer(m_input_coordinate);
#ifdef __ANDROID__
    if (m_texture_type == GPU_TEXTURE_OES){
        glBindTexture(GL_TEXTURE_EXTERNAL_OES, 0);    
    }
#endif
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    context->glContextUnlock();

    vertex_buffer->unLock();
    coor_buffer->unLock();
}

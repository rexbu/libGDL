/**
 * file :	GPUPadding.cpp
 * author :	Rex
 * create :	2018-01-05 17:43
 * func : 
 * history:
 */

#include "GPUPadding.h"
#include "ConvFrameBuffer.h"

const char* g_uint_fragment_shader = SHADER30_STRING(
    in highp vec2 textureCoordinate;
    uniform highp sampler2D inputImageTexture[1];
    out highp uvec4 out_color;
                                                            
#define encodeFloats(v) uvec4(floatBitsToUint(v.r), floatBitsToUint(v.g), floatBitsToUint(v.b), floatBitsToUint(v.a))
#define decodeFloats(v) vec4(uintBitsToFloat(v.r), uintBitsToFloat(v.g), uintBitsToFloat(v.b), uintBitsToFloat(v.a))
    void main()
    {
        vec4 color = texture(inputImageTexture[0], textureCoordinate);
        //out_color = encodeFloats(color);
        out_color = encodeFloats(color);
    }
);

GPUPadding::GPUPadding(int channel_count, int padding_size, conv_pad_t padding_type):
m_padding_size(padding_size),
m_padding_type(padding_type),
GPULayerBase(channel_count){
    m_inputs = 1;
    m_program = new GPUProgram(g_vertext30_shader, g_uint_fragment_shader);
    init();
}

void GPUPadding::render(){
#if DEBUG_FILTER_NAME
    err_log("filter name: %s texture: %d", m_filter_name.c_str(), m_input_buffers[0]->m_texture);
#endif
    
    GPUCheckGlError(m_filter_name.c_str(), true, false);
    GPUContext* context = GPUContext::shareInstance();
    context->glContextLock();   // 加锁，防止此时设置参数
    
    context->setActiveProgram(m_program);
    activeOutFrameBuffer();
    for (int i=0; i<m_inputs; i++) {
        m_input_buffers[i]->activeTexture(GL_TEXTURE0+i);
        glUniform1i(m_input_textures[i], 0+i);
    }
    
    m_coordinate_buffer->activeBuffer(m_input_coordinate);
    m_vertex_buffer->activeBuffer(m_position);
    glDrawElements(GL_TRIANGLES, dynamic_cast<ConvFrameBuffer*>(m_outbuffer)->indexCount(), GL_UNSIGNED_SHORT, ((ConvFrameBuffer*)m_outbuffer)->vertexIndexs());
    glFlush();
    m_coordinate_buffer->disableBuffer(m_input_coordinate);
    m_vertex_buffer->disableBuffer(m_position);
    m_outbuffer->unactive();
    
    GPUCheckGlError(m_filter_name.c_str(), true, false);
    context->glContextUnlock();
}

void GPUPadding::activeOutFrameBuffer(){
    ConvFrameBuffer* input_frame = dynamic_cast<ConvFrameBuffer*>(m_input_buffers[0]);
    if (m_outbuffer == NULL) {
        // 重新设置卷积核数量
        m_kernel_count = m_last_layer->m_kernel_count;
        m_outx_count = input_frame->m_x_count;
        m_outy_count = input_frame->m_y_count;
        
        // m_channel_count在addTarget过程已经被设置
        m_outbuffer = ConvBufferCache::getFrameBuffer(input_frame->m_channel_width, input_frame->m_channel_height, m_padding_size, m_outx_count, m_outy_count);
        setOutputSize(m_outbuffer->m_width, m_outbuffer->m_height);
        
        // 计算顶点坐标
        switch (m_padding_type) {
            case CONV_PADDING_ZERO:
                break;
            case CONV_PADDING_REFLECT:
                // 每个channel有9个正方形，每个正方形有16个坐标点，需要9*6个索引
                m_vertex_buffer = new GPUVertexBuffer(m_outx_count*m_outy_count*36);
                m_vertex_buffer->setBuffer(((ConvFrameBuffer*)m_outbuffer)->reflectVertices());
                m_coordinate_buffer = new GPUVertexBuffer(m_outx_count*m_outy_count*36);
                m_coordinate_buffer->setBuffer(input_frame->reflectCoordinates(m_padding_size));
            default:
                break;
        }
        
        memset(m_clear_color, 0, sizeof(float)*4);
        
        //m_outcoor_buffer = new GPUVertexBuffer(m_channel_count*6);
        //m_outcoor_buffer->setBuffer(&m_out_coordinates[0]);
        
    }
    else{
        m_outbuffer = ConvBufferCache::getFrameBuffer(input_frame->m_channel_width, input_frame->m_channel_height, m_padding_size, m_outx_count, m_outy_count);
    }
    m_outbuffer->activeBuffer();
    
    glClearColor(m_clear_color[0], m_clear_color[1], m_clear_color[2], m_clear_color[3]);
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

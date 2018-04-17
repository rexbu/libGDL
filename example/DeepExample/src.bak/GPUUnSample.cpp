/**
 * file :	GPUUnSample.cpp
 * author :	Rex
 * create :	2018-02-11 00:47
 * func : 
 * history:
 */

#include "GPUUnSample.h"
#include "ConvFrameBuffer.h"

extern const char* g_uint_fragment_shader;
GPUUnSample::GPUUnSample(int size):
m_unsample_size(size),
GPULayerBase(1){
    m_inputs = 1;
    m_program = new GPUProgram(g_vertext30_shader, g_uint_fragment_shader);
    init();
}

void GPUUnSample::activeOutFrameBuffer(){
    if (m_outbuffer == NULL) {
        ConvFrameBuffer* buffer = dynamic_cast<ConvFrameBuffer*>(m_input_buffers[0]);
        m_outbuffer = new ConvFrameBuffer(m_unsample_size*buffer->m_channel_width, m_unsample_size*buffer->m_channel_height, buffer->m_padding_size, buffer->m_x_count, buffer->m_y_count);
        m_outx_count = buffer->m_x_count;
        m_outy_count = buffer->m_y_count;
        m_kernel_count = m_last_layer->m_kernel_count;
        
        m_coordinate_buffer = new GPUVertexBuffer();
        m_vertex_buffer = new GPUVertexBuffer();
        m_vertex_buffer->setBuffer(&m_vertices[0]);
        m_coordinate_buffer->setBuffer(GPUFilter::coordinatesRotation(GPUNoRotation));
    }
    m_outbuffer->activeBuffer();
    
    glClearColor(m_clear_color[0], m_clear_color[1], m_clear_color[2], m_clear_color[3]);
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

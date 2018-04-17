/**
 * file :	GPUAdaIn.cpp
 * author :	Rex
 * create :	2018-02-09 12:56
 * func : 
 * history:
 */

#include "GPUAdaIn.h"
#include "ConvFrameBuffer.h"

static const char* g_adain_fragment = SHADER30_STRING(
    in highp vec2 textureCoordinate;
    out highp uvec4 out_color;
    uniform int channel_size[2];
    // 0为方差，1为均值，2为原始卷积图像,3为style方差，4位style均值
    uniform highp sampler2D inputImageTexture[5];
    uniform highp float output_step[2];
    // input_step是方差步长
    uniform highp float input_step[2];

    #define encodeFloats(v) uvec4(floatBitsToUint((v).r), floatBitsToUint((v).g), floatBitsToUint((v).b), floatBitsToUint((v).a))
    #define decodeFloats(v) vec4(uintBitsToFloat((v).r), uintBitsToFloat((v).g), uintBitsToFloat((v).b), uintBitsToFloat((v).a))

    void main(){
        int wi = int(round(textureCoordinate.x/output_step[0] - 0.5));
        int hi = int(round(textureCoordinate.y/output_step[1] - 0.5));
        int cx = wi/channel_size[0];
        int cy = hi/channel_size[1];
        
        highp vec2 in_coor = vec2(float(cx)*input_step[0]+input_step[0]/2.0, float(cy)*input_step[1]+input_step[1]/2.0);
        highp vec4 sigma_c = texture(inputImageTexture[0], in_coor);
        highp vec4 mean_c = texture(inputImageTexture[1], in_coor);
        highp vec4 sigma_s = texture(inputImageTexture[3], in_coor);
        highp vec4 mean_s = texture(inputImageTexture[4], in_coor);
        
        highp vec4 color = texture(inputImageTexture[2], textureCoordinate);
        highp vec4 normal = (color-mean_c)*sigma_s/sigma_c + mean_s;
        out_color = encodeFloats(normal);
    }
);

GPUAdaIn::GPUAdaIn():
GPULayerBase(1){
    m_inputs = 5;
    m_mean_buffer = NULL;
    m_var_buffer = NULL;
    m_program = new GPUProgram(g_vertext30_shader, g_adain_fragment, m_filter_name.c_str());
    init();
}

GPUAdaIn::~GPUAdaIn(){
    if (m_mean_buffer!=NULL) {
        delete m_mean_buffer;
    }
    if (m_var_buffer!=NULL) {
        delete m_var_buffer;
    }
}

void GPUAdaIn::newFrame(){
    GPULayerBase::newFrame();
    memset(&m_input_buffers[0], 0, sizeof(GPUFrameBuffer*)*3);
}

void GPUAdaIn::setFrameSize(uint32_t width, uint32_t height){
    GPULayerBase::setFrameSize(width, height);
    
    // 2是卷积输入纹理
    ConvFrameBuffer* buffer = dynamic_cast<ConvFrameBuffer*>(m_input_buffers[2]);
    int channel_size[] = {buffer->m_channel_width+2*buffer->m_padding_size, buffer->m_channel_height+2*buffer->m_padding_size};
    m_program->setIntegerv("channel_size", channel_size, 2);
    float out_step[] = {(float)1.0/buffer->m_width, (float)1.0/buffer->m_height};
    m_program->setFloatv("output_step", out_step, 2);
    // input_step是均值方差步长
    buffer = dynamic_cast<ConvFrameBuffer*>(m_input_buffers[0]);
    float texture_step[] = {(float)1.0/buffer->m_width, (float)1.0/buffer->m_height};
    m_program->setFloatv("input_step", texture_step, 2);
}

void GPUAdaIn::activeOutFrameBuffer(){
    if (m_outbuffer == NULL) {
        ConvFrameBuffer* buffer = dynamic_cast<ConvFrameBuffer*>(m_input_buffers[2]);
        m_outbuffer = new ConvFrameBuffer(buffer->m_channel_width, buffer->m_channel_height, buffer->m_padding_size, buffer->m_x_count, buffer->m_y_count);
        
        memset(m_clear_color, 0, sizeof(float)*4);
        m_coordinate_buffer = new GPUVertexBuffer();
        m_coordinate_buffer->setBuffer(GPUFilter::coordinatesRotation(GPUNoRotation));
        m_vertex_buffer = new GPUVertexBuffer();
        m_vertex_buffer->setBuffer(&m_vertices[0]);
    }
    m_outbuffer->activeBuffer();
    
    glClearColor(m_clear_color[0], m_clear_color[1], m_clear_color[2], m_clear_color[3]);
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

void GPUAdaIn::setStyle(float* mean, float* var, int width, int height){
    m_var_buffer = floatFrameBuffer(var, width, height);
    m_input_buffers[3] = m_var_buffer;
    m_mean_buffer = floatFrameBuffer(mean, width, height);
    m_input_buffers[4] = m_mean_buffer;
}

void GPUAdaIn::diff(){
    if (m_values == NULL) {
        return;
    }
    
    float* rgba = (float*)malloc(m_outbuffer->m_width*m_outbuffer->m_height*4*4);
    m_outbuffer->getPixels((uint8_t*)rgba);
    ConvFrameBuffer* buffer = dynamic_cast<ConvFrameBuffer*>(m_outbuffer);
    for (int hc=0; hc<buffer->m_y_count; hc++) {
        for (int wc=0; wc<buffer->m_x_count; wc++) {
            int x_start = wc*(buffer->m_channel_width);
            int y_start = hc*(buffer->m_channel_height);
            for (int h=0; h<buffer->m_channel_height; h++) {
                for (int w=0; w<buffer->m_channel_width; w++) {
                    float r = rgba[(h+y_start)*m_outbuffer->m_width*4+(w+x_start)*4];
                    float g = rgba[(h+y_start)*m_outbuffer->m_width*4+(w+x_start)*4+1];
                    float b = rgba[(h+y_start)*m_outbuffer->m_width*4+(w+x_start)*4+2];
                    float a = rgba[(h+y_start)*m_outbuffer->m_width*4+(w+x_start)*4+3];
                    
                    float f1 = m_values[(hc*buffer->m_x_count + wc)*(buffer->m_channel_width*buffer->m_channel_height)*4 + h*buffer->m_channel_width+w];
                    float f2 = m_values[(hc*buffer->m_x_count + wc)*(buffer->m_channel_width*buffer->m_channel_height)*4 + (buffer->m_channel_width*buffer->m_channel_height) + h*buffer->m_channel_width+w];
                    float f3 = m_values[(hc*buffer->m_x_count + wc)*(buffer->m_channel_width*buffer->m_channel_height)*4 + (buffer->m_channel_width*buffer->m_channel_height)*2 + h*buffer->m_channel_width+w];
                    float f4 = m_values[(hc*buffer->m_x_count + wc)*(buffer->m_channel_width*buffer->m_channel_height)*4 + (buffer->m_channel_width*buffer->m_channel_height)*3 + h*buffer->m_channel_width+w];
                    if (abs(r-f1)>0.01) {
                        printf("diff pos[r:%d/%d/%d] val[%f, %f]\n", (hc*buffer->m_x_count+wc)*4, w,h, r, f1);
                    }
                    if (abs(g-f2)>0.01) {
                        printf("diff pos[g:%d/%d/%d] val[%f, %f]\n", (hc*buffer->m_x_count+wc)*4+1, w,h, g, f2);
                    }
                    if (abs(b-f3)>0.01) {
                        printf("diff pos[b:%d/%d/%d] val[%f, %f]\n", (hc*buffer->m_x_count+wc)*4+2, w,h, b, f3);
                    }
                    if (abs(a-f4)>0.01) {
                        printf("diff pos[a:%d/%d/%d] val[%f, %f]\n", (hc*buffer->m_x_count+wc)*4+3, w,h, a, f4);
                    }
                }
            }
        }
    }
    
    free(rgba);
}

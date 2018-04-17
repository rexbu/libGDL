/**
 * file :	GPUVariance.cpp
 * author :	Rex
 * create :	2018-02-08 22:50
 * func : 
 * history:
 */

#include "GPUVariance.h"
#include "ConvFrameBuffer.h"

static const char* g_variance_fragment = SHADER30_STRING(
    in highp vec2 textureCoordinate;
    out highp uvec4 out_color;
    const int channel_width = %d;
    const int channel_height = %d;
    // 1为输入纹理，0为均值纹理
    uniform highp sampler2D inputImageTexture[2];
    uniform highp float output_step[2];
    uniform highp float input_step[2];
                                                     
#define encodeFloats(v) uvec4(floatBitsToUint((v).r), floatBitsToUint((v).g), floatBitsToUint((v).b), floatBitsToUint((v).a))
#define decodeFloats(v) vec4(uintBitsToFloat((v).r), uintBitsToFloat((v).g), uintBitsToFloat((v).b), uintBitsToFloat((v).a))
                                                     
    void main(){
        highp vec4 mean = texture(inputImageTexture[0], textureCoordinate);
        
        highp float wi = round(textureCoordinate.x/output_step[0] - 0.5);
        highp float hi = round(textureCoordinate.y/output_step[1] - 0.5);
        highp float startx = wi*float(channel_width)*input_step[0] + input_step[0]/2.0;
        highp float starty = hi*float(channel_height)*input_step[1] + input_step[1]/2.0;
        highp vec4 total = vec4(0.0, 0.0, 0.0, 0.0);
        for(int h=0; h<channel_height; h++){
            for(int w=0; w<channel_width; w++){
                highp vec4 color = texture(inputImageTexture[1], vec2(startx, starty) + vec2(float(w)*input_step[0], float(h)*input_step[1]));
                total += (color-mean)*(color-mean);
            }
        }
        
        total = sqrt(total/float(channel_width*channel_height)+0.00001);
        out_color = encodeFloats(total);
        // out_color = texture(inputImageTexture[0], pos.rg);
    }
);

GPUVariance::GPUVariance():GPULayerBase(1){
    setInputs(2);
    m_fill_mode = GPUFillModePreserveAspectRatio;
}

void GPUVariance::setFrameSize(uint32_t width, uint32_t height){
    
    vector<GPUFrameBuffer*> input_buffers = m_input_buffers;
    ConvFrameBuffer* buffer = dynamic_cast<ConvFrameBuffer*>(m_input_buffers[1]);
    char shader[10240];
    snprintf(shader, 10240, g_variance_fragment, buffer->m_channel_width, buffer->m_channel_height);
    m_program = new GPUProgram(g_vertext30_shader, shader, "GPUVariance");
    init();
    // init函数会清掉m_input_buffers
    m_input_buffers = input_buffers;
    
    GPULayerBase::setFrameSize(width, height);
    
    float texture_step[] = {(float)1.0/buffer->m_width, (float)1.0/buffer->m_height};
    m_program->setFloatv("input_step", texture_step, 2);
    float out_step[] = {(float)1.0/buffer->m_x_count, (float)1.0/buffer->m_y_count};
    m_program->setFloatv("output_step", out_step, 2);
}

void GPUVariance::activeOutFrameBuffer(){
    // 1是卷积输入纹理
    ConvFrameBuffer* buffer = dynamic_cast<ConvFrameBuffer*>(m_input_buffers[1]);
    if (m_outbuffer == NULL) {
        m_outbuffer = ConvBufferCache::getFrameBuffer(buffer->m_x_count, buffer->m_y_count, 0, 1, 1);
        
        memset(m_clear_color, 0, sizeof(float)*4);
        m_coordinate_buffer = new GPUVertexBuffer();
        m_coordinate_buffer->setBuffer(GPUFilter::coordinatesRotation(GPUNoRotation));
        m_vertex_buffer = new GPUVertexBuffer();
        m_vertex_buffer->setBuffer(&m_vertices[0]);
    }
    else{
        m_outbuffer = ConvBufferCache::getFrameBuffer(buffer->m_x_count, buffer->m_y_count, 0, 1, 1);
    }
    
    m_outbuffer->activeBuffer();
    
    glClearColor(m_clear_color[0], m_clear_color[1], m_clear_color[2], m_clear_color[3]);
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

void GPUVariance::diff(){
    if (m_values == NULL) {
        return;
    }
    
    float* rgba = (float*)malloc(m_outbuffer->m_width*m_outbuffer->m_height*4*4);
    m_outbuffer->getPixels((uint8_t*)rgba);
    for (int i=0; i<m_outbuffer->m_width*m_outbuffer->m_height; i++) {
        float r = rgba[i];
        float f1 = m_values[i];
        if (abs(r-f1)>0.001) {
            printf("diff pos[%d] val[%f, %f]\n", i, r, f1);
        }
    }
    free(rgba);
}

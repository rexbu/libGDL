/**
 * file :	GPUMean.cpp
 * author :	Rex
 * create :	2018-02-08 22:49
 * func : 
 * history:
 */

#include "GPUMean.h"
#include "ConvFrameBuffer.h"

static const char* g_mean_fragment = SHADER30_STRING(
    in highp vec2 textureCoordinate;
    out highp uvec4 out_color;
    // 0为输入纹理，1为输出对应的输入纹理位置
    uniform highp sampler2D inputImageTexture[1];
    const int channel_width = %d;
    const int channel_height = %d;
    uniform highp float output_step[2];
    uniform highp float input_step[2];
                                                         
#define encodeFloats(v) uvec4(floatBitsToUint((v).r), floatBitsToUint((v).g), floatBitsToUint((v).b), floatBitsToUint((v).a))
#define decodeFloats(v) vec4(uintBitsToFloat((v).r), uintBitsToFloat((v).g), uintBitsToFloat((v).b), uintBitsToFloat((v).a))
                                                         
    void main(){
        highp float wi = round(textureCoordinate.x/output_step[0] - 0.5);
        highp float hi = round(textureCoordinate.y/output_step[1] - 0.5);
        highp float startx = wi*float(channel_width)*input_step[0] + input_step[0]/2.0;
        highp float starty = hi*float(channel_height)*input_step[1] + input_step[1]/2.0;
        highp vec4 total = vec4(0.0, 0.0, 0.0, 0.0);
        // 当channel_width和channel_height为变量时候，opengl看起来对循环次数做了限制，256就会溢出，所以这里为常量
        for (highp int h=0; h< channel_height; h++){
            for(highp int w=0; w< channel_width; w++){
                highp vec4 color = texture(inputImageTexture[0], vec2(startx, starty) + vec2(float(w)*input_step[0], float(h)*input_step[1]));
                total += color;
            }
        }
        
        highp int size = channel_width*channel_height;
        total = total/float(size);
        //total = vec4(float(size));
        out_color = encodeFloats(total);
    }
);

GPUMean::GPUMean():GPULayerBase(1, "Mean"){
    setInputs(1);
    m_fill_mode = GPUFillModePreserveAspectRatio;
}

void GPUMean::setFrameSize(uint32_t width, uint32_t height){
    GPUFilter::setFrameSize(width, height);
    
    ConvFrameBuffer* buffer = dynamic_cast<ConvFrameBuffer*>(m_last_layer->m_outbuffer);
    char shader[10240];
    snprintf(shader, 10240, g_mean_fragment, buffer->m_channel_width, buffer->m_channel_height);
    m_program = new GPUProgram(g_vertext30_shader, shader, "GPUMean");
    init();
    // init函数会清掉m_input_buffers
    m_input_buffers[0] = buffer;
    
    float texture_step[] = {(float)1.0/buffer->m_width, (float)1.0/buffer->m_height};
    m_program->setFloatv("input_step", texture_step, 2);
    float out_step[] = {(float)1.0/buffer->m_x_count, (float)1.0/buffer->m_y_count};
    m_program->setFloatv("output_step", out_step, 2);
}

void GPUMean::activeOutFrameBuffer(){
    if (m_outbuffer==NULL) {
        ConvFrameBuffer* buffer = dynamic_cast<ConvFrameBuffer*>(m_input_buffers[0]);
        m_outbuffer = new ConvFrameBuffer(buffer->m_x_count, buffer->m_y_count, 0, 1, 1);
        
        m_coordinate_buffer = new GPUVertexBuffer();
        m_vertex_buffer = new GPUVertexBuffer();
        m_vertex_buffer->setBuffer(&m_vertices[0]);
        m_coordinate_buffer->setBuffer(GPUFilter::coordinatesRotation(GPUNoRotation));
    }
    
    m_outbuffer->activeBuffer();
    glClearColor(m_clear_color[0], m_clear_color[1], m_clear_color[2], m_clear_color[3]);
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

void GPUMean::diff(){
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

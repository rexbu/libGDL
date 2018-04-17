/**
 * file :	GPUTest.cpp
 * author :	Rex
 * create :	2017-07-27 18:36
 * func : 
 * history:
 */

#include "GPUTest.h"
const static char* g_test_fragment_shader = SHADER30_STRING(
    in highp vec2 textureCoordinate;
    out highp vec4 out_color;
    uniform sampler2D inputImageTexture;

    highp vec4 encodeFloat(highp float v ) {
        highp uint uv = floatBitsToUint(v);
        highp uvec4 uvbit = uvec4(uv/uint(256*256*256), uv/uint(256*256), uv/uint(256), uv);
        uvbit -= uvbit.xxyz * uvec4(0, 256, 256, 256);
        return vec4(float(uvbit.x)/255.0, float(uvbit.y)/255.0, float(uvbit.z)/255.0, float(uvbit.w)/255.0);
    }
    
    highp float decodeFloat(highp vec4 v ) {
        highp uint uv = uint(v.x*255.0)*uint(256*256*256) + uint(v.y*255.0)*uint(256*256) + uint(v.z*255.0)*uint(256) + uint(v.w*255.0);
        return uintBitsToFloat(uv);
    }
    void main()
    {
        out_color = encodeFloat(textureCoordinate.x);
    }
);

GPUTestFilter::GPUTestFilter():GPUFilter(g_vertext30_shader, g_test_fragment_shader, 1){
    w = 4096;
    h = 10;
    setOutputSize(w, h);
    m_buffer = new unsigned char[w*h*4];
    m_option = GPUFrameBuffer::nearestFrameOption();
    memset(m_clear_color, 0, sizeof(float)*4);
}

void GPUTestFilter::newFrame(){
    GPUFrameBuffer buffer(10, 10);
    setInputFrameBuffer(&buffer);
    GPUFilter::newFrame();
    m_outbuffer->getPixels(m_buffer);
    int h = 0;
    float step  = 1.0/m_outbuffer->m_width;
    for (int w=0; w<m_outbuffer->m_width; w++) {
        uint32_t r = m_buffer[h*m_outbuffer->m_width*4+w*4];
        uint32_t g = m_buffer[h*m_outbuffer->m_width*4+w*4+1];
        uint32_t b = m_buffer[h*m_outbuffer->m_width*4+w*4+2];
        uint32_t a = m_buffer[h*m_outbuffer->m_width*4+w*4+3];
        uint32_t ii = r*256*256*256 + g*256*256 + b*256 + a;
        float f;
        memcpy(&f, &ii, sizeof(float));
        
        int i = round((f - step/2.0)/step);
        if (i!=w) {
            printf("%f %f i:%d w:%d\n", step, f, i, w);
        }
    }
    printf("\n");
}

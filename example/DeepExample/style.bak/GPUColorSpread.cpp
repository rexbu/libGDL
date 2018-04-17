/**
 * file :	GPUColorSpread.cpp
 * author :	Rex
 * create :	2018-01-25 17:23
 * func : 
 * history:
 */

#include "GPUColorSpread.h"

static const char* g_spread_shpae_fragment = SHADER30_STRING(
    // 纹理坐标，用于获取颜色
    in highp vec2 textureCoordinate;
    out highp uvec4 out_color;
    uniform highp sampler2D inputImageTexture[1];
    uniform int channel_count;
/*
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
*/
#define encodeFloats(v) uvec4(floatBitsToUint((v).r), floatBitsToUint((v).g), floatBitsToUint((v).b), floatBitsToUint((v).a))
#define decodeFloats(v) vec4(uintBitsToFloat((v).r), uintBitsToFloat((v).g), uintBitsToFloat((v).b), uintBitsToFloat((v).a))
    void main(){
        highp vec4 color = texture(inputImageTexture[0], textureCoordinate);
        // 将a设置为0，这样第一层卷积计算不用考虑a
        color = color*255.0 - vec4(123.68, 116.779, 103.939, 255.0);
        out_color = encodeFloats(color.bgra);
    }
);

GPUColorSpread::GPUColorSpread(int channel_count):
m_channel_count(channel_count),
GPULayerBase(channel_count, "ColorSpread"){
    m_inputs = 1;
    m_program = new GPUProgram(g_vertext30_shader, g_spread_shpae_fragment, "GPUConvShape");
    init();
    // setInteger("channel_count", channel_count);
    m_fill_mode = GPUFillModePreserveAspectRatio;
}

/*
void GPUColorSpread::newFrame(){
    render();
    unlockInputFrameBuffers();
    // print();
}
*/

void GPUColorSpread::activeOutFrameBuffer(){
    if (m_outbuffer==NULL) {
        m_outbuffer = new ConvFrameBuffer(m_frame_width, m_frame_height, 0, 1, 1);
        
        m_coordinate_buffer = new GPUVertexBuffer();
        m_vertex_buffer = new GPUVertexBuffer();
        m_vertex_buffer->setBuffer(&m_vertices[0]);
        m_coordinate_buffer->setBuffer(GPUFilter::coordinatesRotation(GPUNoRotation));
        
    }
    
    m_outbuffer->activeBuffer();
    glClearColor(m_clear_color[0], m_clear_color[1], m_clear_color[2], m_clear_color[3]);
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

void GPUColorSpread::print(){
    printf("##### %d/%d ####\n", m_outbuffer->m_width, m_outbuffer->m_height);
    uint8_t* rgba = (uint8_t*)malloc(m_outbuffer->m_width*m_outbuffer->m_height*4);
    m_outbuffer->getPixels(rgba);
    for (int h=0; h<m_outbuffer->m_height; h++) {
        for (int w=0; w<m_outbuffer->m_width; w++) {
            uint32_t r = rgba[h*m_outbuffer->m_width*4+w*4];
            uint32_t g = rgba[h*m_outbuffer->m_width*4+w*4+1];
            uint32_t b = rgba[h*m_outbuffer->m_width*4+w*4+2];
            uint32_t a = rgba[h*m_outbuffer->m_width*4+w*4+3];
            uint32_t ii = r*256*256*256 + g*256*256 + b*256 + a;
            float f;
            memcpy(&f, &ii, sizeof(float));
            printf("%d\t", int(f));
        }
        printf("\n");
    }
    
    free(rgba);
}

/**
 * file :	GPUDeColor.cpp
 * author :	Rex
 * create :	2018-02-11 01:27
 * func : 
 * history:
 */

#include "GPUDeColor.h"

static const char* g_decolor_fragment = SHADER30_STRING(
    // 纹理坐标，用于获取颜色
    in highp vec2 textureCoordinate;
    out highp vec4 out_color;
    uniform highp sampler2D inputImageTexture[1];
    uniform int channel_count;

    void main(){
        highp vec4 color = texture(inputImageTexture[0], textureCoordinate);
        // 将a设置为0，这样第一层卷积计算不用考虑a
        color += vec4(103.939, 116.779, 123.68, 0.0);
        color.a = 255.0;
        out_color = color.bgra/255.0;
    }
);

GPUDeColor::GPUDeColor():
GPULayerBase(1){
    m_inputs = 1;
    m_program = new GPUProgram(g_vertext30_shader, g_decolor_fragment, "DeColor");
    init();
    // setInteger("channel_count", channel_count);
    m_fill_mode = GPUFillModePreserveAspectRatio;
}

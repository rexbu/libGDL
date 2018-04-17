/**
 * file :	GPUBlendFilter.cpp
 * author :	Rex
 * email : rex@labjk.com
 * create :	2016-07-29 14:36
 * func : 
 * history:
 */

#include "GPUBlendFilter.h"

const char* GPUBlendFilter::g_mutable_vertext_shader = SHADER_STRING(
 attribute vec4 position;
 attribute vec4 inputTextureCoordinate;
 attribute vec4 inputTextureCoordinate2;
 attribute vec4 inputTextureCoordinate3;
 attribute vec4 inputTextureCoordinate4;
 attribute vec4 inputTextureCoordinate5;
 attribute vec4 inputTextureCoordinate6;
 attribute vec4 inputTextureCoordinate7;
 attribute vec4 inputTextureCoordinate8;

 varying vec2 textureCoordinate;
 varying vec2 textureCoordinate2;
 varying vec2 textureCoordinate3;
 varying vec2 textureCoordinate4;
 varying vec2 textureCoordinate5;
 varying vec2 textureCoordinate6;
 varying vec2 textureCoordinate7;
 varying vec2 textureCoordinate8;
 void main()
 {
     gl_Position = position;
     textureCoordinate = inputTextureCoordinate.xy;
     textureCoordinate2 = inputTextureCoordinate2.xy;
     textureCoordinate3 = inputTextureCoordinate3.xy;
     textureCoordinate4 = inputTextureCoordinate4.xy;
     textureCoordinate5 = inputTextureCoordinate5.xy;
     textureCoordinate6 = inputTextureCoordinate6.xy;
     textureCoordinate7 = inputTextureCoordinate7.xy;
     textureCoordinate8 = inputTextureCoordinate8.xy;
 }
);

const char* GPUBlendFilter::g_mutable_fragment_shader = SHADER_STRING(
 varying mediump vec2 textureCoordinate;
 varying mediump vec2 textureCoordinate2;
 varying mediump vec2 textureCoordinate3;
 varying mediump vec2 textureCoordinate4;
 varying mediump vec2 textureCoordinate5;
 varying mediump vec2 textureCoordinate6;
 varying mediump vec2 textureCoordinate7;
 varying mediump vec2 textureCoordinate8;

 uniform sampler2D inputImageTexture;
 uniform sampler2D inputImageTexture2;
 uniform sampler2D inputImageTexture3;
 uniform sampler2D inputImageTexture4;
 uniform sampler2D inputImageTexture5;
 uniform sampler2D inputImageTexture6;
 uniform sampler2D inputImageTexture7;
 uniform sampler2D inputImageTexture8;
 
 uniform mediump float alpha[8];
 void main()
 {
     mediump vec4 o1 = texture2D(inputImageTexture, textureCoordinate);
     mediump vec4 o2 = texture2D(inputImageTexture2,textureCoordinate2);
     mediump vec4 o3 = texture2D(inputImageTexture3,textureCoordinate3);
     mediump vec4 o4 = texture2D(inputImageTexture4,textureCoordinate4);
     mediump vec4 o5 = texture2D(inputImageTexture5,textureCoordinate5);
     mediump vec4 o6 = texture2D(inputImageTexture6,textureCoordinate6);
     mediump vec4 o7 = texture2D(inputImageTexture7,textureCoordinate7);
     mediump vec4 o8 = texture2D(inputImageTexture8,textureCoordinate8);
     
     mediump float a8 = alpha[7]*o8.a;
     mediump float a7 = alpha[6]*o7.a*(1.0-a8);
     mediump float a6 = alpha[5]*o6.a*(1.0-a7-a8);
     mediump float a5 = alpha[4]*o5.a*(1.0-a6-a7-a8);
     mediump float a4 = alpha[3]*o4.a*(1.0-a5-a6-a7-a8);
     mediump float a3 = alpha[2]*o3.a*(1.0-a4-a5-a6-a7-a8);
     mediump float a2 = alpha[1]*o2.a*(1.0-a3-a4-a5-a6-a7-a8);
     mediump float a1 = alpha[0]*o1.a*(1.0-a2-a3-a4-a5-a6-a7-a8);
     
     mediump vec4 bk = a1*o1 + ceil(a2)*o2 + ceil(a3)*o3 + ceil(a4)*o4 + ceil(a5)*o5 + ceil(a6)*o6 + ceil(a7)*o7 + ceil(a8)*o8;
     gl_FragColor = bk;
 }
);

GPUBlendFilter::GPUBlendFilter(int inputs):
GPUFilter(g_mutable_vertext_shader, g_mutable_fragment_shader, inputs){
    char name[64];
    for (int i = m_inputs; i < GPU_MUTABLE_TEXTURE_NUM; ++i){
        sprintf(name, "inputTextureCoordinate%d", i+1);
        m_input_coordinate = m_program->attributeIndex(name);
        if (m_input_coordinate >= 0){
            glEnableVertexAttribArray(m_input_coordinate);
        }
        else{
            err_log("Filter[%s] get coordinate%d error", m_filter_name.c_str(), i+1);
        }
        
        sprintf(name, "inputImageTexture%d", i+1);
        m_input_textures[i] = m_program->uniformIndex(name);
        if (m_input_textures[i]<0)
        {
            err_log("Filter[%s] get textures%d error", m_filter_name.c_str(), i);
        }
    }
}

GPUBlendFilter::GPUBlendFilter(const char* fragment, int inputs):
GPUFilter(g_mutable_vertext_shader, fragment, inputs){
    char name[64];
    for (int i = m_inputs; i < GPU_MUTABLE_TEXTURE_NUM; ++i){
        sprintf(name, "inputTextureCoordinate%d", i+1);
        m_input_coordinate = m_program->attributeIndex(name);
        if (m_input_coordinate >= 0){
            glEnableVertexAttribArray(m_input_coordinate);
        }
        else{
            err_log("Filter[%s] get coordinate%d error", m_filter_name.c_str(), i+1);
        }
        
        sprintf(name, "inputImageTexture%d", i+1);
        m_input_textures[i] = m_program->uniformIndex(name);
        if (m_input_textures[i]<0)
        {
            err_log("Filter[%s] get textures%d error", m_filter_name.c_str(), i);
        }
    }
}

GPUBlendFilter::GPUBlendFilter(const char* vertex, const char* fragment, int inputs):
GPUFilter(vertex, fragment, inputs){
    char name[64];
    for (int i = m_inputs; i < GPU_MUTABLE_TEXTURE_NUM; ++i){
        sprintf(name, "inputTextureCoordinate%d", i+1);
        m_input_coordinate = m_program->attributeIndex(name);
        if (m_input_coordinate >= 0){
            glEnableVertexAttribArray(m_input_coordinate);
        }
        else{
            err_log("Filter[%s] get coordinate%d error", m_filter_name.c_str(), i+1);
        }
        
        sprintf(name, "inputImageTexture%d", i+1);
        m_input_textures[i] = m_program->uniformIndex(name);
        if (m_input_textures[i]<0)
        {
            err_log("Filter[%s] get textures%d error", m_filter_name.c_str(), i);
        }
    }
}

void GPUBlendFilter::newFrame(){
    float enableAlpha[8] = {0};
    for (int i = 0; i < m_inputs; ++i)
    {
        enableAlpha[i] = 1.0;
    }
    setFloatv("alpha", enableAlpha, m_inputs);
    
    GPUFilter::newFrame();
}

void GPUBlendFilter::setDrawRect(gpu_point_t points[4], int index){
    if (index >= GPU_MUTABLE_TEXTURE_NUM) {
        err_log("Visionin Error: index out of range");
        return;
    }
    
    // 计算在points作为四个顶点的坐标系下，00 10 01 11的坐标
    float unit_x = sqrt((points[1].x-points[0].x)*(points[1].x-points[0].x)+(points[1].y-points[0].y)*(points[1].y-points[0].y));
    float unit_y = sqrt((points[2].x-points[0].x)*(points[2].x-points[0].x)+(points[2].y-points[0].y)*(points[2].y-points[0].y));
    
    const float* coordinate = GPUFilter::coordinatesRotation(m_rotation);
    for (int i=0; i<4; i++) {
        // 左上
        if (coordinate[i*2]==0 && coordinate[i*2+1]==0) {
            m_coordinates[index][i*2] = (0-points[0].x)/unit_x;
            m_coordinates[index][i*2+1] = (0-points[0].y)/unit_y;
        }
        // 右上
        else if (coordinate[i*2]==1 && coordinate[i*2+1]==0) {
            m_coordinates[index][i*2] = 1+(1-points[1].x)/unit_x;
            m_coordinates[index][i*2+1] = (0-points[1].y)/unit_y;
        }
        // 左下
        else if (coordinate[i*2]==0 && coordinate[i*2+1]==1) {
            m_coordinates[index][i*2] = (0-points[2].x)/unit_x;
            m_coordinates[index][i*2+1] = 1+(1-points[2].y)/unit_y;
        }
        // 右下
        else if (coordinate[i*2]==1 && coordinate[i*2+1]==1) {
            m_coordinates[index][i*2] = 1+(1-points[3].x)/unit_x;
            m_coordinates[index][i*2+1] = 1+(1-points[3].y)/unit_y;
        }
    }
}

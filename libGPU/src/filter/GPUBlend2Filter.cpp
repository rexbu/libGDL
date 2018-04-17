/**
 * file :	GPUBlend2Filter.cpp
 * author :	Rex
 * create :	2017-04-06 17:21
 * func : 
 * history:
 */

#include "GPUBlend2Filter.h"

const char* g_blend_fragment_shader = SHADER_STRING(
 varying mediump vec2 textureCoordinate;
 varying mediump vec2 textureCoordinate2;

 uniform sampler2D inputImageTexture[2];

 void main()
 {
     mediump vec4 o1 = texture2D(inputImageTexture[0], textureCoordinate);
     mediump vec4 o2 = texture2D(inputImageTexture[1],textureCoordinate2);
     
     mediump vec4 bk = (1.0-o2.a)*o1+o2.a*o2;
     gl_FragColor = bk;
 }
);

GPUBlend2Filter::GPUBlend2Filter():
GPUFilter(g_blend_fragment_shader, 2, "Blend2Filter")
{
    m_blend_pic = NULL;
    disable();
}
GPUBlend2Filter::~GPUBlend2Filter(){
    if (m_blend_pic!=NULL) {
        delete m_blend_pic;
    }
}

void GPUBlend2Filter::setBlendImage(GPUPicture* pic, gpu_rect_t rect, bool mirror){
    gpu_point_t points[4];
    points[0].x = rect.pointer.x;
    points[0].y = rect.pointer.y;
    points[1].x = rect.pointer.x + rect.size.width;
    points[1].y = rect.pointer.y;
    points[2].x = rect.pointer.x;
    points[2].y = rect.pointer.y + rect.size.height;
    points[3].x = rect.pointer.x + rect.size.width;
    points[3].y = rect.pointer.y + rect.size.height;
    setBlendImagePoints(pic, points, mirror);
}

void GPUBlend2Filter::setBlendImagePoints(GPUPicture* pic, gpu_point_t points[4], bool mirror){
    if (m_blend_pic!=NULL) {
        m_blend_pic->removeAllTargets();
        delete m_blend_pic;
        m_blend_pic = NULL;
    }
    
    // 停止使用
    if (pic==NULL) {
        disable();
    }
    
    // 计算在points作为四个顶点的坐标系下，00 10 01 11的坐标
    float unit_x = sqrt((points[1].x-points[0].x)*(points[1].x-points[0].x)+(points[1].y-points[0].y)*(points[1].y-points[0].y));
    float unit_y = sqrt((points[2].x-points[0].x)*(points[2].x-points[0].x)+(points[2].y-points[0].y)*(points[2].y-points[0].y));
    
    const float* coordinate = GPUFilter::coordinatesRotation(m_rotation);
    for (int i=0; i<4; i++) {
        // 左上
        if (coordinate[i*2]==0 && coordinate[i*2+1]==0) {
            m_coordinates[i*2] = (0-points[0].x)/unit_x;
            m_coordinates[i*2+1] = (0-points[0].y)/unit_y;
        }
        // 右上
        else if (coordinate[i*2]==1 && coordinate[i*2+1]==0) {
            m_coordinates[i*2] = 1+(1-points[1].x)/unit_x;
            m_coordinates[i*2+1] = (0-points[1].y)/unit_y;
        }
        // 左下
        else if (coordinate[i*2]==0 && coordinate[i*2+1]==1) {
            m_coordinates[i*2] = (0-points[2].x)/unit_x;
            m_coordinates[i*2+1] = 1+(1-points[2].y)/unit_y;
        }
        // 右下
        else if (coordinate[i*2]==1 && coordinate[i*2+1]==1) {
            m_coordinates[i*2] = 1+(1-points[3].x)/unit_x;
            m_coordinates[i*2+1] = 1+(1-points[3].y)/unit_y;
        }
    }
    
    if (mirror) {
        GLfloat coors[8];
        coors[0] = m_coordinates[2];
        coors[1] = m_coordinates[3];
        coors[2] = m_coordinates[0];
        coors[3] = m_coordinates[1];
        coors[4] = m_coordinates[6];
        coors[5] = m_coordinates[7];
        coors[6] = m_coordinates[4];
        coors[7] = m_coordinates[5];
        memcpy(m_coordinates, coors, sizeof(GLfloat)*8);
    }
    
    m_blend_pic = pic;
    GPUContext* context = GPUContext::shareInstance();
    context->glContextLock();
    pic->addTarget(this, 1);
    context->glContextUnlock();
    enable();
}

void GPUBlend2Filter::setInputFrameBuffer(GPUFrameBuffer *buffer, int location){
    if (location==0 && !m_disable) {
        m_blend_pic->processImage();
    }
    else if(location==0 && m_disable){
        // 为了让ready返回true
        GPUFilter::setInputFrameBuffer(buffer, 1);
    }
    
    GPUFilter::setInputFrameBuffer(buffer, location);
}

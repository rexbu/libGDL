/**
 * file :	GPUYUVFilter.cpp
 * author :	Rex
 * email : rex@labjk.com
 * create :	2016-05-03 15:54
 * func : 
 * history:
 */

#include "GPUYUVFilter.h"

#ifndef SATURATE
#define SATURATE(x, min, max)	((x) < (min) ? (min) : (x) > (max) ? (max) : (x))
#endif
/////////////////// NV21转YUV ///////////////////////
static const char* g_nv21toyuv_fragment_shader = SHADER_STRING(
	varying mediump vec2 textureCoordinate;
    varying mediump vec2 textureCoordinate2;
    uniform sampler2D inputImageTexture;
    uniform sampler2D inputImageTexture2;

	void main()
	{
        mediump vec3 yuv;
        mediump vec3 rgb;
         
        yuv.x = texture2D(inputImageTexture, textureCoordinate).r;
        yuv.zy = texture2D(inputImageTexture2, textureCoordinate).ra;
         
        gl_FragColor = vec4(yuv, 1.0);
	}
);

GPUNV21ToYUVFilter::GPUNV21ToYUVFilter():
GPUFilter(g_nv21toyuv_fragment_shader, 2){
    m_filter_name = "NV21ToYUVFilter";
}
GPUNV21ToYUVFilter::GPUNV21ToYUVFilter(const char* fragment):GPUFilter(fragment, 2){
    m_filter_name = "NV21ToYUVFilter";
}

//// NV21转YUV带美白
static const char* g_nv212yuv_eq_fragment = SHADER_STRING
(
 varying mediump vec2 textureCoordinate;
 varying mediump vec2 textureCoordinate2;
 uniform sampler2D inputImageTexture;
 uniform sampler2D inputImageTexture2;
 
 uniform mediump vec4 threshold;    /// luma equalization thresholds
 uniform mediump vec4 slope;        /// luma equalization slopes
 
 uniform mediump mat3 rgb2yuv;
 uniform mediump vec3 off2yuv;
 
 void main()
 {
     mediump vec3 yuv;
     
     yuv.x = texture2D(inputImageTexture, textureCoordinate).r;
     yuv.zy = texture2D(inputImageTexture2, textureCoordinate).ra;

     mediump float luma = yuv.r;
     mediump float u = yuv.g;
     mediump float v = yuv.b;
     
     mediump float l0 =   min(luma, threshold.x);
     mediump float l1 = clamp(luma, threshold.x, threshold.y)-threshold.x;
     mediump float l2 = clamp(luma, threshold.y, threshold.z)-threshold.y;
     mediump float l3 = clamp(luma, threshold.z, threshold.w)-threshold.z;
     mediump float l4 =   max(luma, threshold.w)             -threshold.w;
     luma = l0+
     l1*slope.y+
     l2*slope.z+
     l3*slope.w+
     l4;

     gl_FragColor = vec4(luma, u, v, 1.0);
 }
 );

GPUNV21ToYUVFilter::GPUNV21ToYUVFilter(float s, int cc):
GPUFilter(g_nv212yuv_eq_fragment, 2){
    m_strength = s;
    
    float ideal[6] = {0.2, 0.6, 0.7, 0.8, 0.5, 0.6};
    setIdeal(ideal);
    //    float thresh[4] = {0.4, 0.5, 0.6, 0.9};
    //    setThreshold(thresh);
    
    if(!cc) {
        GLfloat mtx[] = {1,0,0,   0,1,0,   0,0,1},
        off[] = {0,0,0};
        memcpy(&m_rgb2yuv, mtx, sizeof(GLfloat)*9);
        memcpy(&m_off2yuv, off, sizeof(GLfloat)*3);
    } else {
        GLfloat mtx[] = {0.2984, -0.1690, 0.5012, 0.5875, -0.3328, -0.4196, 0.1142, 0.5019, -0.0815},
        off[] = {0.0, 0.5, 0.5};
        memcpy(&m_rgb2yuv, mtx, sizeof(GLfloat)*9);
        memcpy(&m_off2yuv, off, sizeof(GLfloat)*3);
    }
    
    setMatrix("rgb2yuv", m_rgb2yuv, 3);
    setFloat("off2yuv", m_off2yuv, 3);
    
    m_filter_name = "NV21ToYUVFilter";
}
void GPUNV21ToYUVFilter::setIdeal(float ideal[4]) {
    memcpy(m_idealpnts, ideal, 6*sizeof(float));
    m_idealpnts[6] = ideal[4];
    m_idealpnts[7] = ideal[5];
    m_threshold[0] = ideal[0];
    m_threshold[1] = ideal[4];
    m_threshold[2] = ideal[5];
    m_threshold[3] = ideal[3];
    setStrength(m_strength);
}
void GPUNV21ToYUVFilter::setThreshold(float thres[4]){
    const float D = .01;
    float s, min, max;
    
    s = m_strength*.2;
    min = m_idealpnts[0]*s+(1-s)*m_idealpnts[4];
    s = m_strength*.3;
    max = m_idealpnts[5]*s+(1-s)*m_idealpnts[4];
    if(thres[1]-D > m_idealpnts[6])
        s = m_threshold[1]+D;
    else if(thres[1]+D < m_idealpnts[6])
        s = m_threshold[1]-D;
    m_threshold[1] = SATURATE(s, min, max);
    
    s = m_strength*.3;
    min = m_idealpnts[4]*s+(1-s)*m_idealpnts[5];
    s = m_strength*.5;
    max = m_idealpnts[3]*s+(1-s)*m_idealpnts[5];
    if(thres[2]-D > m_idealpnts[7])
        s = m_threshold[2]+D;
    else if(thres[2]+D < m_idealpnts[7])
        s = m_threshold[2]-D;
    m_threshold[2] = SATURATE(s, min, max);
    
    /// [self setFloatVec4:threshold forUniform:@"threshold"];
    setStrength(m_strength);
}

void GPUNV21ToYUVFilter::setStrength(float s){
    info_log("NV21 Brighten level:%f", s);
    if(s < 0)
        s = m_strength;
    m_strength = s;
    float t = 1-s;
    m_idealpnts[6] = m_idealpnts[1]*s+t*m_idealpnts[4];
    m_idealpnts[7] = m_idealpnts[2]*s+t*m_idealpnts[5];
    
    float s0 = 1;
    float s1 = (m_idealpnts[6]-m_idealpnts[0])/(m_threshold[1]-m_threshold[0]);
    float s2 = (m_idealpnts[7]-m_idealpnts[6])/(m_threshold[2]-m_threshold[1]);
    float s3 = (m_idealpnts[3]-m_idealpnts[7])/(m_threshold[3]-m_threshold[2]);
    
    GLfloat slope[] = {s0,s1,s2,s3};
    setFloat("slope", slope, 4);
    setFloat("threshold", m_threshold, 4);
}

/////////////////// NV12转YUV ///////////////////////
static const char* g_nv12toyuv_fragment_shader = SHADER_STRING(
    varying mediump vec2 textureCoordinate;
    varying mediump vec2 textureCoordinate2;
    uniform sampler2D inputImageTexture;
    uniform sampler2D inputImageTexture2;

    void main()
    {
        mediump vec3 yuv;
        mediump vec3 rgb;
         
        yuv.x = texture2D(inputImageTexture, textureCoordinate).r;
        yuv.yz = texture2D(inputImageTexture2, textureCoordinate).ra;         
        gl_FragColor = vec4(yuv, 1.0);
    }
);

static const char* g_nv122yuv_eq_fragment = SHADER_STRING
(
 varying mediump vec2 textureCoordinate;
 varying mediump vec2 textureCoordinate2;
 uniform sampler2D inputImageTexture;
 uniform sampler2D inputImageTexture2;
 
 uniform mediump vec4 threshold;    /// luma equalization thresholds
 uniform mediump vec4 slope;        /// luma equalization slopes
 
 uniform mediump mat3 rgb2yuv;
 uniform mediump vec3 off2yuv;
 
 void main()
 {
     mediump vec3 yuv;
     
     yuv.x = texture2D(inputImageTexture, textureCoordinate).r;
     yuv.yz = texture2D(inputImageTexture2, textureCoordinate).ra;

     mediump float luma = yuv.r;
     mediump float u = yuv.g;
     mediump float v = yuv.b;
     
     mediump float l0 =   min(luma, threshold.x);
     mediump float l1 = clamp(luma, threshold.x, threshold.y)-threshold.x;
     mediump float l2 = clamp(luma, threshold.y, threshold.z)-threshold.y;
     mediump float l3 = clamp(luma, threshold.z, threshold.w)-threshold.z;
     mediump float l4 =   max(luma, threshold.w)             -threshold.w;
     luma = l0+
     l1*slope.y+
     l2*slope.z+
     l3*slope.w+
     l4;

     gl_FragColor = vec4(luma, u, v, 1.0);
 }
 );
GPUNV12ToYUVFilter::GPUNV12ToYUVFilter():GPUNV21ToYUVFilter(g_nv12toyuv_fragment_shader){
    m_filter_name = "NV12ToYUVFilter";
}

GPUNV12ToYUVFilter::GPUNV12ToYUVFilter(float s, int cc):GPUNV21ToYUVFilter(g_nv122yuv_eq_fragment){
    m_strength = s;
    
    float ideal[6] = {0.2, 0.6, 0.7, 0.8, 0.5, 0.6};
    setIdeal(ideal);
    //    float thresh[4] = {0.4, 0.5, 0.6, 0.9};
    //    setThreshold(thresh);
    
    if(!cc) {
        GLfloat mtx[] = {1,0,0,   0,1,0,   0,0,1},
        off[] = {0,0,0};
        memcpy(&m_rgb2yuv, mtx, sizeof(GLfloat)*9);
        memcpy(&m_off2yuv, off, sizeof(GLfloat)*3);
    } else {
        GLfloat mtx[] = {0.2984, -0.1690, 0.5012, 0.5875, -0.3328, -0.4196, 0.1142, 0.5019, -0.0815},
        off[] = {0.0, 0.5, 0.5};
        memcpy(&m_rgb2yuv, mtx, sizeof(GLfloat)*9);
        memcpy(&m_off2yuv, off, sizeof(GLfloat)*3);
    }
    
    setMatrix("rgb2yuv", m_rgb2yuv, 3);
    setFloat("off2yuv", m_off2yuv, 3);
    
    m_filter_name = "NV12ToYUVFilter";
}

#pragma --mark "NV21转RGB"
static const char* g_nv21torgb_vertex_shader = SHADER_STRING(
    attribute vec4 position;
    attribute vec4 inputTextureCoordinate;
    attribute vec4 inputTextureCoordinate2;
    varying vec2 textureCoordinate;
    varying vec2 textureCoordinate2;
    void main()
    {
        gl_Position = position;
        textureCoordinate = inputTextureCoordinate.xy;
        textureCoordinate2 = inputTextureCoordinate2.xy;
    }
);

static const char* g_nv21torgb_fragment_shader = SHADER_STRING(
	varying mediump vec2 textureCoordinate;
    uniform sampler2D inputImageTexture[2];
    mediump mat3 colorConversionMatrix = mat3(1.0, 1.0, 1.0, 0.0, -0.343, 1.765, 1.4, -0.711, 0.0);

	void main()
	{
        mediump vec3 yuv;
        mediump vec3 rgb;
         
        yuv.x = texture2D(inputImageTexture[0], textureCoordinate).r;
        yuv.zy = texture2D(inputImageTexture[1], textureCoordinate).ra - vec2(0.5, 0.5);
        rgb = colorConversionMatrix * yuv;
         
        gl_FragColor = vec4(rgb, 1.0);
	}
);

GPUNV21ToRGBFilter::GPUNV21ToRGBFilter():
GPUFilter(g_nv21torgb_vertex_shader, g_nv21torgb_fragment_shader, 2){
}

#pragma --mark "NV12转RGB"
static const char* g_nv12torgb_fragment_shader = SHADER_STRING(
   varying mediump vec2 textureCoordinate;
   varying mediump vec2 textureCoordinate2;
   uniform sampler2D inputImageTexture;
   uniform sampler2D inputImageTexture2;
   mediump mat3 colorConversionMatrix = mat3(1.0, 1.0, 1.0, 0.0, -0.343, 1.765, 1.4, -0.711, 0.0);
   
   void main()
   {
       mediump vec3 yuv;
       mediump vec3 rgb;
       
       yuv.x = texture2D(inputImageTexture, textureCoordinate).r;
       yuv.yz = texture2D(inputImageTexture2, textureCoordinate).ra - vec2(0.5, 0.5);
       rgb = colorConversionMatrix * yuv;
       
       gl_FragColor = vec4(rgb, 1.0);
   }
);

GPUNV12ToRGBFilter::GPUNV12ToRGBFilter():
GPUFilter(g_nv12torgb_fragment_shader, 2, "NV12ToRGBFilter"){
}

#pragma --mark "I420转RGB"
static const char* g_i420torgb_fragment_shader = SHADER_STRING(
   varying mediump vec2 textureCoordinate;
   varying mediump vec2 textureCoordinate2;
   varying mediump vec2 textureCoordinate3;
   uniform sampler2D inputImageTexture;
   uniform sampler2D inputImageTexture2;
   uniform sampler2D inputImageTexture3;
   mediump mat3 colorConversionMatrix = mat3(1.0, 1.0, 1.0, 0.0, -0.343, 1.765, 1.4, -0.711, 0.0);
   
   void main()
   {
       mediump vec3 yuv;
       mediump vec3 rgb;
       
       yuv.x = texture2D(inputImageTexture, textureCoordinate).r;
       yuv.y = texture2D(inputImageTexture2, textureCoordinate).r - 0.5;
       yuv.z = texture2D(inputImageTexture3, textureCoordinate).r - 0.5;
       rgb = colorConversionMatrix * yuv;
       
       gl_FragColor = vec4(rgb, 1.0);
   }
);
GPUI420ToRGBFilter::GPUI420ToRGBFilter():
GPUFilter(g_i420torgb_fragment_shader, 3, "GPUI420ToRGBFilter"){
}
////////////////////// YUV转RGB ////////////////////////////
static const char* g_yuv2rgb_fragment = SHADER_STRING
(
 varying mediump vec2 textureCoordinate;
 uniform sampler2D inputImageTexture;
 /// const mediump mat3 RGBtoYCbCr = mat3(0.2984, -0.1690, 0.5012, 0.5875, -0.3328, -0.4196, 0.1142, 0.5019, -0.0815);
 const mediump mat3 YCbCrtoRGB = mat3(1.0, 1.0, 1.0, 0.0, -0.343, 1.765, 1.4, -0.711, 0.0);
 void main()
 {
    mediump vec4 color = texture2D(inputImageTexture, textureCoordinate);    
    gl_FragColor = vec4(YCbCrtoRGB * (color.rgb-vec3(0.0, 0.5, 0.5)), color.a);
 }
 );

GPUYUVToRGBFilter::GPUYUVToRGBFilter():
GPUFilter(g_yuv2rgb_fragment){
    m_filter_name = "YUVToRGBFilter";
}

////////////////////// RGB转YUV ////////////////////////////
static const char* g_rgb2yuv_fragment = SHADER_STRING
(
 varying mediump vec2 textureCoordinate;
 uniform sampler2D inputImageTexture;
 const mediump mat3 RGB2YUV = mat3(0.2984, -0.1690, 0.5012, 0.5875, -0.3328, -0.4196, 0.1142, 0.5019, -0.0815);
 const mediump mat3 I = mat3(0.5 ,0.0 , 0.0 ,   0.0, 0.5, 0.0,   0.0, 0.0, 0.5);
 /// const mediump mat3 YUV2RGB = mat3(1.0, 1.0, 1.0, 0.0, -0.343, 1.765, 1.4, -0.711, 0.0);
 void main()
 {
    mediump vec4 color = texture2D(inputImageTexture, textureCoordinate);
    gl_FragColor = vec4(RGB2YUV * color.rgb+vec3(0.0, 0.5, 0.5), color.a);
 }
 );

static const char* g_rgb2yuv_eq_fragment = SHADER_STRING
(
 varying mediump vec2 textureCoordinate;
 
 uniform sampler2D inputImageTexture;
 uniform mediump vec4 threshold;    /// luma equalization thresholds
 uniform mediump vec4 slope;        /// luma equalization slopes
 
 uniform mediump mat3 rgb2yuv;
 uniform mediump vec3 off2yuv;
 
 void main()
 {
     mediump vec4 color = texture2D(inputImageTexture, textureCoordinate);
     mediump vec3 yuv = rgb2yuv*color.rgb+off2yuv;
     
     mediump float luma = yuv.r;
     mediump float u = yuv.g;
     mediump float v = yuv.b;
     
     mediump float l0 =   min(luma, threshold.x);
     mediump float l1 = clamp(luma, threshold.x, threshold.y)-threshold.x;
     mediump float l2 = clamp(luma, threshold.y, threshold.z)-threshold.y;
     mediump float l3 = clamp(luma, threshold.z, threshold.w)-threshold.z;
     mediump float l4 =   max(luma, threshold.w)             -threshold.w;
     luma = l0+
     l1*slope.y+
     l2*slope.z+
     l3*slope.w+
     l4;
     
     gl_FragColor = vec4(luma, u, v, color.a);
 }
 );

GPURGBToYUVFilter::GPURGBToYUVFilter():
GPUFilter(g_rgb2yuv_fragment){
    m_filter_name = "RGBToYUVFilter";
}

GPURGBToYUVFilter::GPURGBToYUVFilter(float s, int cc):
GPUFilter(g_rgb2yuv_eq_fragment){
    m_strength = s;
    
    float ideal[6] = {0.2, 0.6, 0.7, 0.8, 0.5, 0.6};
    setIdeal(ideal);
//    float thresh[4] = {0.4, 0.5, 0.6, 0.9};
//    setThreshold(thresh);
    
    if(!cc) {
        GLfloat mtx[] = {1,0,0,   0,1,0,   0,0,1},
        off[] = {0,0,0};
        memcpy(&m_rgb2yuv, mtx, sizeof(GLfloat)*9);
        memcpy(&m_off2yuv, off, sizeof(GLfloat)*3);
    } else {
        GLfloat mtx[] = {0.2984, -0.1690, 0.5012, 0.5875, -0.3328, -0.4196, 0.1142, 0.5019, -0.0815},
        off[] = {0.0, 0.5, 0.5};
        memcpy(&m_rgb2yuv, mtx, sizeof(GLfloat)*9);
        memcpy(&m_off2yuv, off, sizeof(GLfloat)*3);
    }
    
    setMatrix("rgb2yuv", m_rgb2yuv, 3);
    setFloat("off2yuv", m_off2yuv, 3);
    
    m_filter_name = "RGBToYUVFilter";
}

void GPURGBToYUVFilter::setIdeal(float ideal[6]) {
    memcpy(m_idealpnts, ideal, 6*sizeof(float));
    m_idealpnts[6] = ideal[4];
    m_idealpnts[7] = ideal[5];
    m_threshold[0] = ideal[0];
    m_threshold[1] = ideal[4];
    m_threshold[2] = ideal[5];
    m_threshold[3] = ideal[3];
    setStrength(m_strength);
}
void GPURGBToYUVFilter::setThreshold(float thres[4]){
    const float D = .01;
    float s, min, max;
    
    s = m_strength*.2;
    min = m_idealpnts[0]*s+(1-s)*m_idealpnts[4];
    s = m_strength*.3;
    max = m_idealpnts[5]*s+(1-s)*m_idealpnts[4];
    if(thres[1]-D > m_idealpnts[6])
        s = m_threshold[1]+D;
    else if(thres[1]+D < m_idealpnts[6])
        s = m_threshold[1]-D;
    m_threshold[1] = SATURATE(s, min, max);
    
    s = m_strength*.3;
    min = m_idealpnts[4]*s+(1-s)*m_idealpnts[5];
    s = m_strength*.5;
    max = m_idealpnts[3]*s+(1-s)*m_idealpnts[5];
    if(thres[2]-D > m_idealpnts[7])
        s = m_threshold[2]+D;
    else if(thres[2]+D < m_idealpnts[7])
        s = m_threshold[2]-D;
    m_threshold[2] = SATURATE(s, min, max);
    
    /// [self setFloatVec4:threshold forUniform:@"threshold"];
    setStrength(m_strength);
}

void GPURGBToYUVFilter::setStrength(float s){
    info_log("RGB Brighten level:%f", s);
    if(s < 0)
        s = m_strength;
    m_strength = s;
    float t = 1-s;
    m_idealpnts[6] = m_idealpnts[1]*s+t*m_idealpnts[4];
    m_idealpnts[7] = m_idealpnts[2]*s+t*m_idealpnts[5];
    
    float s0 = 1;
    float s1 = (m_idealpnts[6]-m_idealpnts[0])/(m_threshold[1]-m_threshold[0]);
    float s2 = (m_idealpnts[7]-m_idealpnts[6])/(m_threshold[2]-m_threshold[1]);
    float s3 = (m_idealpnts[3]-m_idealpnts[7])/(m_threshold[3]-m_threshold[2]);
    
    GLfloat slope[] = {s0,s1,s2,s3};
    setFloat("slope", slope, 4);
    setFloat("threshold", m_threshold, 4);
}

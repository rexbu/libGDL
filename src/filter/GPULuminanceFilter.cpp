/**
 * file :	GPULuminanceFilter.cpp
 * author :	Rex
 * create :	2016-08-05 23:08
 * func : 
 * history:
 */

#include "GPULuminanceFilter.h"

// rgb转灰度一般为 ITU HDTV标准，系数(0.2125, 0.7154, 0.0721)。CRT显示器非线性色彩系数为(0.299, 0.587, 0.114)
const static char* g_luminance_fragment_shader = SHADER_STRING
(
 varying vec2 textureCoordinate;
 uniform sampler2D inputImageTexture;
 const highp vec3 W = vec3(0.2125, 0.7154, 0.0721);
 
 void main()
 {
     mediump vec4 textureColor = texture2D(inputImageTexture, textureCoordinate);
     highp float luminance = dot(textureColor.rgb, W);
     
     gl_FragColor = vec4(vec3(luminance), textureColor.a);
 }
 );

GPULuminanceFilter::GPULuminanceFilter():GPUFilter(g_luminance_fragment_shader){}

// GPULuminanceRangeFilter
const static char* g_luminancerange_fragment_shader = SHADER_STRING
(
 varying highp vec2 textureCoordinate;
 
 uniform sampler2D inputImageTexture;
 uniform lowp float rangeReduction;
 
 // Values from "Graphics Shaders: Theory and Practice" by Bailey and Cunningham
 const mediump vec3 luminanceWeighting = vec3(0.2125, 0.7154, 0.0721);
 
 void main()
 {
     lowp vec4 textureColor = texture2D(inputImageTexture, textureCoordinate);
     mediump float luminance = dot(textureColor.rgb, luminanceWeighting);
     mediump float luminanceRatio = ((0.5 - luminance) * rangeReduction);
     
     gl_FragColor = vec4((textureColor.rgb) + (luminanceRatio), textureColor.w);
 }
 );

GPULuminanceRangeFilter::GPULuminanceRangeFilter(float range):
GPUFilter(g_luminancerange_fragment_shader){
    setExtraParameter(range);
}

void GPULuminanceRangeFilter::setExtraParameter(float r){
    setFloat("rangeReduction", r);
}

GPUExtraBlurFilter::GPUExtraBlurFilter(){
    m_input = &m_saturation_filter;
    m_output = &m_luminance_filter;
    
    m_input->addTarget(&m_gaussian_filter);
    m_gaussian_filter.addTarget(m_output);
    
    m_saturation_filter.setExtraParameter(0.8);
    m_gaussian_filter.setExtraParameter(48.0);
    m_luminance_filter.setExtraParameter(0.6);
}

void GPUExtraBlurFilter::setExtraParameter(float r){
    m_gaussian_filter.setExtraParameter(r);
}
/**
 * file :	GPUSaturationFilter.cpp
 * author :	Rex
 * create :	2016-08-07 11:13
 * func : 
 * history:
 */

#include "GPUSaturationFilter.h"

const static char* g_saturation_fragment_shader = SHADER_STRING
(
 varying highp vec2 textureCoordinate;
 
 uniform sampler2D inputImageTexture;
 uniform lowp float saturation;
 
 // Values from "Graphics Shaders: Theory and Practice" by Bailey and Cunningham
 const mediump vec3 luminanceWeighting = vec3(0.2125, 0.7154, 0.0721);
 
 void main()
 {
     lowp vec4 textureColor = texture2D(inputImageTexture, textureCoordinate);
     lowp float luminance = dot(textureColor.rgb, luminanceWeighting);
     lowp vec3 greyScaleColor = vec3(luminance);
     
     gl_FragColor = vec4(mix(greyScaleColor, textureColor.rgb, saturation), textureColor.w);
     
 }
 );

GPUSaturationFilter::GPUSaturationFilter(float sa):GPUFilter(g_saturation_fragment_shader){
    setExtraParameter(sa);
}

void GPUSaturationFilter::setExtraParameter(float saturation){
    setFloat("saturation", saturation);
}
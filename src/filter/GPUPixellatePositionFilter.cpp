//
//  GPUPixellatePositionFilter.cpp
//  libGPU
//
//  Created by zhoubl on 2017/9/25.
//  Copyright © 2017年 Rex. All rights reserved.
//

#include "GPUPixellatePositionFilter.h"

const static char* g_positionmosaic_fragment_shader = SHADER_STRING (
   varying highp vec2 textureCoordinate;
   
   uniform sampler2D inputImageTexture;
   
   uniform highp float fractionalWidthOfPixel;
   uniform highp float aspectRatio;
   uniform lowp vec2 pixelateCenter;
   uniform highp float pixelateRadius;
   
   void main()
   {
     highp vec2 textureCoordinateToUse = vec2(textureCoordinate.x, (textureCoordinate.y * aspectRatio + 0.5 - 0.5 * aspectRatio));
     highp float dist = distance(pixelateCenter, textureCoordinateToUse);
     
     if (dist < pixelateRadius)
     {
       gl_FragColor = texture2D(inputImageTexture, textureCoordinate );
     }
     else
     {
       highp vec2 sampleDivisor = vec2(fractionalWidthOfPixel, fractionalWidthOfPixel / aspectRatio);
       highp vec2 samplePos = textureCoordinate - mod(textureCoordinate, sampleDivisor) + 0.5 * sampleDivisor;
       gl_FragColor = texture2D(inputImageTexture, samplePos );
     }
   }
 );

GPUPixellatePositionFilter::GPUPixellatePositionFilter(float blockSize, float ratio):GPUFilter(g_positionmosaic_fragment_shader) {
  m_filter_name = "GPUPixellatePositionFilter";
  setFloat("fractionalWidthOfPixel", blockSize);
  setFloat("aspectRatio", ratio);
  
  GLfloat pixelateCenter[] = {0.5, 0.5};
  setFloat("pixelateCenter", pixelateCenter, 2);
  setFloat("pixelateRadius", 0.5);
}

void GPUPixellatePositionFilter::setExtraParameter(float blockSize) {
  setFloat("fractionalWidthOfPixel", blockSize);
}

void GPUPixellatePositionFilter::adjustPixellate(gpu_point_t center, float radius) {
    GLfloat pixelateCenter[] = {center.x, center.y};
    setFloat("pixelateCenter", pixelateCenter, 2);
    setFloat("pixelateRadius", radius);
}


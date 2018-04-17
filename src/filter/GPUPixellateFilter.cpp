//
//  GPUPixellateFilter.cpp
//  libGPU
//
//  Created by zhoubl on 2017/9/26.
//  Copyright © 2017年 Rex. All rights reserved.
//

#include "GPUPixellateFilter.h"


const static char* g_mosaic_fragment_shader = SHADER_STRING (
  varying highp vec2 textureCoordinate;
  
  uniform sampler2D inputImageTexture;
  
  uniform highp float fractionalWidthOfPixel;
  uniform highp float aspectRatio;
  
  void main()
  {
    highp vec2 sampleDivisor = vec2(fractionalWidthOfPixel, fractionalWidthOfPixel / aspectRatio);
    
    highp vec2 samplePos = textureCoordinate - mod(textureCoordinate, sampleDivisor) + 0.5 * sampleDivisor;
    gl_FragColor = texture2D(inputImageTexture, samplePos );
  }
);

GPUPixellateFilter::GPUPixellateFilter(float blockSize, float ratio):GPUFilter(g_mosaic_fragment_shader) {
  setFloat("fractionalWidthOfPixel", blockSize);
  setFloat("aspectRatio", ratio);
}

void GPUPixellateFilter::setExtraParameter(float blockSize) {
  setFloat("fractionalWidthOfPixel", blockSize);
}

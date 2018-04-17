/**
 * file :	GPUBilateralBlurFilter.cpp
 * author :	Rex
 * create :	2017-05-26 10:34
 * func : 
 * history:
 */

#include "GPUBilateralBlurFilter.h"

const static char* g_bilateral_vertex_shader = SHADER_STRING(
    attribute vec4 position;
    attribute vec4 inputTextureCoordinate;

    const int GAUSSIAN_SAMPLES = 5;

    uniform float texelWidthOffset;
    uniform float texelHeightOffset;

    varying vec2 textureCoordinate;
    varying vec2 blurCoordinates[GAUSSIAN_SAMPLES];

    void main()
    {
     gl_Position = position;
     textureCoordinate = inputTextureCoordinate.xy;
     
     // Calculate the positions for the blur
     int multiplier = 0;
     vec2 blurStep;
     vec2 singleStepOffset = vec2(texelWidthOffset, texelHeightOffset);
     
     for (int i = 0; i < GAUSSIAN_SAMPLES; i++)
     {
         multiplier = (i - ((GAUSSIAN_SAMPLES - 1) / 2));
         // Blur in x (horizontal)
         blurStep = float(multiplier) * singleStepOffset;
         blurCoordinates[i] = inputTextureCoordinate.xy + blurStep;
     }
    }
);

const static char* g_bilateral_fragment_shader = SHADER_STRING(
    uniform sampler2D inputImageTexture;

    const lowp int GAUSSIAN_SAMPLES = 5;

    varying highp vec2 textureCoordinate;
    varying highp vec2 blurCoordinates[GAUSSIAN_SAMPLES];

    uniform mediump float distanceNormalizationFactor;

    void main()
    {
        lowp vec4 centralColor;
        lowp float gaussianWeightTotal;
        lowp vec4 sum;
        lowp vec4 sampleColor;
        lowp float distanceFromCentralColor;
        lowp float gaussianWeight;

        centralColor = texture2D(inputImageTexture, blurCoordinates[2]);
        gaussianWeightTotal = 0.18;
        sum = centralColor * 0.18;

        sampleColor = texture2D(inputImageTexture, blurCoordinates[0]);
        distanceFromCentralColor = min(distance(centralColor, sampleColor) * distanceNormalizationFactor, 1.0);
        gaussianWeight = 0.05 * (1.0 - distanceFromCentralColor);
        gaussianWeightTotal += gaussianWeight;
        sum += sampleColor * gaussianWeight;

        sampleColor = texture2D(inputImageTexture, blurCoordinates[1]);
        distanceFromCentralColor = min(distance(centralColor, sampleColor) * distanceNormalizationFactor, 1.0);
        gaussianWeight = 0.09 * (1.0 - distanceFromCentralColor);
        gaussianWeightTotal += gaussianWeight;
        sum += sampleColor * gaussianWeight;

        sampleColor = texture2D(inputImageTexture, blurCoordinates[3]);
        distanceFromCentralColor = min(distance(centralColor, sampleColor) * distanceNormalizationFactor, 1.0);
        gaussianWeight = 0.12 * (1.0 - distanceFromCentralColor);
        gaussianWeightTotal += gaussianWeight;
        sum += sampleColor * gaussianWeight;

        sampleColor = texture2D(inputImageTexture, blurCoordinates[4]);
        distanceFromCentralColor = min(distance(centralColor, sampleColor) * distanceNormalizationFactor, 1.0);
        gaussianWeight = 0.15 * (1.0 - distanceFromCentralColor);
        gaussianWeightTotal += gaussianWeight;
        sum += sampleColor * gaussianWeight;
        /*
        sampleColor = texture2D(inputImageTexture, blurCoordinates[5]);
        distanceFromCentralColor = min(distance(centralColor, sampleColor) * distanceNormalizationFactor, 1.0);
        gaussianWeight = 0.15 * (1.0 - distanceFromCentralColor);
        gaussianWeightTotal += gaussianWeight;
        sum += sampleColor * gaussianWeight;

        sampleColor = texture2D(inputImageTexture, blurCoordinates[6]);
        distanceFromCentralColor = min(distance(centralColor, sampleColor) * distanceNormalizationFactor, 1.0);
        gaussianWeight = 0.12 * (1.0 - distanceFromCentralColor);
        gaussianWeightTotal += gaussianWeight;
        sum += sampleColor * gaussianWeight;

        sampleColor = texture2D(inputImageTexture, blurCoordinates[7]);
        distanceFromCentralColor = min(distance(centralColor, sampleColor) * distanceNormalizationFactor, 1.0);
        gaussianWeight = 0.09 * (1.0 - distanceFromCentralColor);
        gaussianWeightTotal += gaussianWeight;
        sum += sampleColor * gaussianWeight;
        
        sampleColor = texture2D(inputImageTexture, blurCoordinates[8]);
        distanceFromCentralColor = min(distance(centralColor, sampleColor) * distanceNormalizationFactor, 1.0);
        gaussianWeight = 0.05 * (1.0 - distanceFromCentralColor);
        gaussianWeightTotal += gaussianWeight;
        sum += sampleColor * gaussianWeight;
         */
        gl_FragColor = sum / gaussianWeightTotal;
    }
);

GPUBilateralBlurFilter::GPUBilateralBlurFilter():
GPUTwoPassFilter(g_bilateral_vertex_shader, g_bilateral_fragment_shader, g_bilateral_vertex_shader, g_bilateral_fragment_shader)
{
    setExtraParameter(8.0);
}

void GPUBilateralBlurFilter::setExtraParameter(float strength){
    m_first_filter->setFloat("distanceNormalizationFactor", strength);
    m_second_filter->setFloat("distanceNormalizationFactor", strength);
}

void GPUBilateralBlurFilter::setInputFrameBuffer(GPUFrameBuffer *buffer, int location){
    GPUTwoPassFilter::setInputFrameBuffer(buffer, location);
    
    // 如果尺寸发生变化，重新设置尺寸
    if (m_frame_width!=buffer->m_width || m_frame_height!=buffer->m_height)
    {
        m_frame_width = buffer->m_width;
        m_frame_height = buffer->m_height;
        m_first_filter->setFloat("texelWidthOffset", 1.0/(m_frame_width-1));
        m_first_filter->setFloat("texelHeightOffset", 0.0);
        m_second_filter->setFloat("texelWidthOffset", 0.0);
        m_second_filter->setFloat("texelHeightOffset", 1.0/(m_frame_height-1));
    }
}

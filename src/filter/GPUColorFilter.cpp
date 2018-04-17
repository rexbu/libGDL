/**
 * file :	GPUContrastFilter.cpp
 * author :	Rex
 * create :	2017-05-26 12:13
 * func : 
 * history:
 */

#include "GPUColorFilter.h"

const static char* g_contrast_fragemtn_shader = SHADER_STRING(
	varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;

    void main()
    {
    	mediump vec4 color = texture2D(inputImageTexture, textureCoordinate);
        mediump float r = abs(2.0*color.g-color.b+color.r)*color.r;
        mediump float g = abs(2.0*color.b-color.g+color.r)*color.r;
        mediump float b = abs(2.0*color.b-color.g+color.r)*color.g;
        
        r = max(0.0, min(r, 1.0));
        g = max(0.0, min(g, 1.0));
        b = max(0.0, min(b, 1.0));
        
        mediump float gray = (r+g+b)/3.0;
        r = min(1.0, gray+10.0/255.0);
        gl_FragColor = vec4(gray, r, r, 1.0);
    }
);

GPUContrastFilter::GPUContrastFilter():
GPUFilter(g_contrast_fragemtn_shader){
}


const static char* g_binary_fragemtn_shader = SHADER_STRING(
    varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;
    uniform mediump float threshold;
    void main()
    {
        mediump vec4 color = texture2D(inputImageTexture, textureCoordinate);
        
        if (color.r>threshold)
        {
            gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);    
        }
        else{
            gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);       
        }
    }
);

GPUBinaryFilter::GPUBinaryFilter():GPUFilter(g_binary_fragemtn_shader){
    setExtraParameter(0.6);
}

void GPUBinaryFilter::setExtraParameter(float p){
    setFloat("threshold", p);
}

const static char* g_salt_fragemtn_shader = SHADER_STRING(
    varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;
    
    void main()
    {
        mediump vec4 color = texture2D(inputImageTexture, textureCoordinate);
        
        if (color.r < 0.8)
        {
            gl_FragColor = vec4(21.0/255.0, 25.0/255.0, 34.0/255.0, 1.0) * (1.0 + color.r);
        }
        else{
            gl_FragColor = vec4(0.9294, 0.9176, 0.8824, 1.0);
        }
    }
);

GPUSaltFilter::GPUSaltFilter():GPUFilter(g_salt_fragemtn_shader){
}

const static char* g_convolution9_fragemtn_shader = SHADER_STRING(
  varying vec2 textureCoordinate;
   uniform sampler2D inputImageTexture;
   
   uniform mediump float wStep;
   uniform mediump float hStep;
   
   void main()
   {
       mediump vec4 colors[5];
       colors[0] = texture2D(inputImageTexture, textureCoordinate + (-4.0)*vec2(wStep, hStep));
       colors[1] = texture2D(inputImageTexture, textureCoordinate + (-2.0)*vec2(wStep, hStep));
       colors[2] = texture2D(inputImageTexture, textureCoordinate);
       colors[3] = texture2D(inputImageTexture, textureCoordinate + (2.0)*vec2(wStep, hStep));
       colors[4] = texture2D(inputImageTexture, textureCoordinate + (4.0)*vec2(wStep, hStep));
       
       mediump vec4 color = (colors[0]+colors[4])*0.00620039 + (colors[1]+colors[3])*0.196125 + colors[2]*0.595349;
       gl_FragColor = color;
   }
);
GPUHConvolution9Filter::GPUHConvolution9Filter():
GPUTwoPassFilter(g_convolution9_fragemtn_shader, g_convolution9_fragemtn_shader){
}

void GPUHConvolution9Filter::setFrameSize(uint32_t width, uint32_t height){
    GPUTwoPassFilter::setFrameSize(width, height);
    m_first_filter->setFloat("wStep", 1.0/(width - 1));
    m_first_filter->setFloat("hStep", 0.0);
    m_second_filter->setFloat("wStep", 0.0);
    m_second_filter->setFloat("hStep", 1.0/(height - 1));
}

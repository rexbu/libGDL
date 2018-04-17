/**
 * file :	GPUMedianFilter.cpp
 * author :	Rex
 * create :	2016-08-05 15:05
 * func : 
 * history:
 */

#include "GPUMedianFilter.h"

const static char* g_median_fragment_shader = SHADER_STRING(
    varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;
    uniform mediump float widthStep;
    uniform mediump float heightStep;
    uniform mediump float radius;
    
    void main()
    {
        mediump vec4 colors[9];
        colors[0] = texture2D(inputImageTexture, textureCoordinate+vec2(-widthStep*radius, radius*heightStep));
        colors[1] = texture2D(inputImageTexture, textureCoordinate+vec2(widthStep*radius, -heightStep*radius));
        colors[2] = texture2D(inputImageTexture, textureCoordinate+vec2(-widthStep*radius, -heightStep*radius));
        colors[3] = texture2D(inputImageTexture, textureCoordinate+vec2(widthStep*radius, heightStep*radius));
        colors[4] = texture2D(inputImageTexture, textureCoordinate+vec2(-widthStep*radius, 0.0));
        colors[5] = texture2D(inputImageTexture, textureCoordinate+vec2(widthStep*radius, 0.0));
        colors[6] = texture2D(inputImageTexture, textureCoordinate+vec2(0.0, heightStep*radius));
        colors[7] = texture2D(inputImageTexture, textureCoordinate+vec2(0.0, -heightStep*radius));
        colors[8] = texture2D(inputImageTexture, textureCoordinate);
        
        for(int i=0; i<9; i++){
            vec4 m = colors[i];
            for(int j=i; j<9; j++){
                if(colors[j].r<colors[i].r){
                    m = colors[i];
                    colors[i] = colors[j];
                    colors[j] = m;
                }
            }
        }
        
        //mediump float c = (colors[0]+colors[1]+colors[2]+colors[3]+colors[4]+colors[5]+colors[6]+colors[7])/8.0;
        int m = 3;
        mediump float c = colors[3].r;
        if(colors[3].r > colors[4].r){
            m = 4;
            c = colors[4].r;
        }
        if(c > colors[5].r){
            m = 5;
            c = colors[5].r;
        }
        gl_FragColor = colors[m];
    }
);

GPUMedianFilter::GPUMedianFilter():GPUFilter(g_median_fragment_shader){
    setFloat("radius", 2.0);
}

void GPUMedianFilter::setFrameSize(uint32_t width, uint32_t height){
    m_frame_width = width;
    m_frame_height = height;
    
    setFloat("widthStep", 1.0/(m_frame_width-1));
    setFloat("heightStep", 1.0/(m_frame_height-1));
}


const static char* g_means_fragment_shader = SHADER_STRING(
    varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;
    uniform mediump float widthStep;
    uniform mediump float heightStep;
    uniform mediump float radius;
    
    void main()
    {
        mediump vec4 colors[9];
        colors[0] = texture2D(inputImageTexture, textureCoordinate+vec2(-widthStep*radius, radius*heightStep));
        colors[1] = texture2D(inputImageTexture, textureCoordinate+vec2(widthStep*radius, -heightStep*radius));
        colors[2] = texture2D(inputImageTexture, textureCoordinate+vec2(-widthStep*radius, -heightStep*radius));
        colors[3] = texture2D(inputImageTexture, textureCoordinate+vec2(widthStep*radius, heightStep*radius));
        colors[4] = texture2D(inputImageTexture, textureCoordinate+vec2(-widthStep*radius, 0.0));
        colors[5] = texture2D(inputImageTexture, textureCoordinate+vec2(widthStep*radius, 0.0));
        colors[6] = texture2D(inputImageTexture, textureCoordinate+vec2(0.0, heightStep*radius));
        colors[7] = texture2D(inputImageTexture, textureCoordinate+vec2(0.0, -heightStep*radius));
        colors[8] = texture2D(inputImageTexture, textureCoordinate);
        
        mediump vec4 c = (colors[0]+colors[1]+colors[2]+colors[3]+colors[4]+colors[5]+colors[6]+colors[7])/8.0;
        gl_FragColor = c;
    }
);

GPUMeansFilter::GPUMeansFilter():GPUFilter(g_means_fragment_shader){
    setFloat("radius", 2.0);
}

void GPUMeansFilter::setFrameSize(uint32_t width, uint32_t height){
    m_frame_width = width;
    m_frame_height = height;
    
    setFloat("widthStep", 1.0/(m_frame_width-1));
    setFloat("heightStep", 1.0/(m_frame_height-1));
}
/*
const static char* g_median_fragment_shader = SHADER_STRING(
    varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;

    uniform mediump float xStep;
    uniform mediump float yStep;
    uniform mediump float distance;
    uniform mediump float step;
    uniform mediump float weight;
    void main()
    {
    	mediump vec4 value = vec4(0.0);
        mediump vec2 stepOffset = vec2(xStep, yStep);
        for(mediump float i = 0.0; i<distance*2.0+1.0; i+=step){
            //value += weight* abs(i-distance) * texture2D(inputImageTexture, textureCoordinate + stepOffset*(i-distance));
            value += weight* texture2D(inputImageTexture, textureCoordinate + stepOffset*(i-distance));
        }
        gl_FragColor = value;
    }
);

GPUMedianFilter::GPUMedianFilter(uint32_t distance):GPUTwoPassFilter(g_median_fragment_shader, g_median_fragment_shader){
	float step = (distance*2.0+1.0)/7;
	float weight = 1.0/7;
	// 最多遍历7个点
	if (step<1)
	{
		step = 1.0;
		weight = 1.0/(2*distance+1);
	}
	m_first_filter->setFloat("distance", distance*1.0);
	m_first_filter->setFloat("step", step);
	m_first_filter->setFloat("weight", weight);
    m_second_filter->setFloat("distance", distance*1.0);
    m_second_filter->setFloat("step", step);
	m_second_filter->setFloat("weight", weight);
}

void GPUMedianFilter::setPixelRadius(uint32_t distance){
    float step = (distance*2.0+1.0)/7;
    float weight = 1.0/7;
    // 最多遍历7个点
    if (step<1)
    {
        step = 1.0;
        weight = 1.0/(2*distance+1);
    }
    m_first_filter->setFloat("distance", distance*1.0);
    m_first_filter->setFloat("step", step);
    m_first_filter->setFloat("weight", weight);
    m_second_filter->setFloat("distance", distance*1.0);
    m_second_filter->setFloat("step", step);
    m_second_filter->setFloat("weight", weight);
}

void GPUMedianFilter::setInputFrameBuffer(GPUFrameBuffer *buffer, int location){
    GPUTwoPassFilter::setInputFrameBuffer(buffer, location);
    
    // 如果尺寸发生变化，重新设置尺寸
    if (m_frame_width!=buffer->m_width || m_frame_height!=buffer->m_height)
    {
        m_frame_width = buffer->m_width;
        m_frame_height = buffer->m_height;
        m_first_filter->setFloat("xStep", 1.0/(m_frame_width-1));
        m_first_filter->setFloat("yStep", 0.0);
        m_second_filter->setFloat("xStep", 0.0);
        m_second_filter->setFloat("yStep", 1.0/(m_frame_height-1));
    }
}
*/

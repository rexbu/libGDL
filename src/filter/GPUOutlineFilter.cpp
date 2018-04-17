/**
 * file :	GPUOutlineFilter.cpp
 * author :	Rex
 * create :	2016-08-16 20:09
 * func : 
 * history:
 */

#include "GPUOutlineFilter.h"

const static char* g_outline_fragment_shader = SHADER_STRING(
	varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;

    uniform mediump float wStep;
    uniform mediump float hStep;
	const mediump float threshold = 0.1;
    bool leftOutline(){
        mediump float i = textureCoordinate.x;
		for (; i >= 0.0 && textureCoordinate.x-i<5.0*wStep; i=i-wStep){
            mediump vec4 c = texture2D(inputImageTexture, vec2(i, textureCoordinate.y));
            if(c.r<=threshold){
                break;
            }
		}

        // 从非边缘开始向左遍历，如果找到边缘，则证明当前边缘不是轮廓，如果遍历到边界没有找到边缘，则证明当前点是轮廓
        mediump float j = i;
        for(; j>=0.0; j=j-wStep){
            mediump vec4 c = texture2D(inputImageTexture, vec2(j, textureCoordinate.y));
            if(c.r>=threshold){
                return false;
            }
        }
        return true;
    }

    bool rightOutline(){
        mediump float i = textureCoordinate.x;
		for (; i <= 1.0 && i-textureCoordinate.x<5.0*wStep ; i=i+wStep){
            mediump vec4 c = texture2D(inputImageTexture, vec2(i, textureCoordinate.y));
            if(c.r<=threshold){
                break;
            }
		}

        // 从非边缘开始向右遍历，如果找到边缘，则证明当前边缘不是轮廓，如果遍历到边界没有找到边缘，则证明当前点是轮廓
        mediump float j = i;
        for(; j<=1.0; j=j+wStep){
            mediump vec4 c = texture2D(inputImageTexture, vec2(j, textureCoordinate.y));
            if(c.r>=threshold){
                return false;
            }
        }
        return true;
    }

    void main()
    {
    	vec4 color = texture2D(inputImageTexture, textureCoordinate);
    	if (color.r>threshold)
    	{
            bool left = leftOutline();
            bool right = rightOutline();
            if (left || right)
            {
            	gl_FragColor = color;
            }
            else{
            	gl_FragColor = vec4(0.0);
            }
    	}
        else{
            gl_FragColor = vec4(0.0);
        }
    }
);

GPUOutlineFilter::GPUOutlineFilter():m_outline_filter(g_outline_fragment_shader){
    m_frame_width = 0;
    m_frame_height = 0;
    m_sobel_filter.setExtraParameter(0.5);
    
    m_input = &m_sobel_filter;
    m_output = &m_outline_filter;
    
    m_sobel_filter.addTarget(&m_outline_filter);
}

void GPUOutlineFilter::setInputFrameBuffer(GPUFrameBuffer *buffer, int location){
    GPUGroupFilter::setInputFrameBuffer(buffer, location);
    // 如果尺寸发生变化，重新设置尺寸
    if (m_frame_width!=buffer->m_width || m_frame_height!=buffer->m_height)
    {
        m_frame_width = buffer->m_width;
        m_frame_height = buffer->m_height;
        m_outline_filter.setFloat("wStep", 1.0/(m_frame_width-1));
        m_outline_filter.setFloat("hStep", 1.0/(m_frame_height-1));
    }
}
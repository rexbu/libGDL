/**
 * file :	GPUSobelEdgeFilter.cpp
 * author :	Rex
 * create :	2016-08-10 20:57
 * func : 
 * history:
 */

#include "GPUSobelEdgeFilter.h"

const static char* g_sobeledge_fragment_shader = SHADER_STRING
(
 varying vec2 textureCoordinate;
 
 uniform sampler2D inputImageTexture;
 uniform mediump float edgeStrength;
 uniform mediump float widthStep;
 uniform mediump float heightStep;
 uniform mediump float threshold;
 
 void main()
 {
    mediump float bottomLeftIntensity = texture2D(inputImageTexture, textureCoordinate+vec2(-widthStep, heightStep)).r;
    mediump float topRightIntensity = texture2D(inputImageTexture, textureCoordinate+vec2(widthStep, -heightStep)).r;
    mediump float topLeftIntensity = texture2D(inputImageTexture, textureCoordinate+vec2(-widthStep, -heightStep)).r;
    mediump float bottomRightIntensity = texture2D(inputImageTexture, textureCoordinate+vec2(widthStep, heightStep)).r;
    mediump float leftIntensity = texture2D(inputImageTexture, textureCoordinate+vec2(-widthStep, 0.0)).r;
    mediump float rightIntensity = texture2D(inputImageTexture, textureCoordinate+vec2(widthStep, 0.0)).r;
    mediump float bottomIntensity = texture2D(inputImageTexture, textureCoordinate+vec2(0.0, heightStep)).r;
    mediump float topIntensity = texture2D(inputImageTexture, textureCoordinate+vec2(0.0, -heightStep)).r;
     
    mediump float h = -topLeftIntensity - 2.0 * topIntensity - topRightIntensity + bottomLeftIntensity + 2.0 * bottomIntensity + bottomRightIntensity;
    mediump float v = -bottomLeftIntensity - 2.0 * leftIntensity - topLeftIntensity + bottomRightIntensity + 2.0 * rightIntensity + topRightIntensity;
    
    mediump float mag = length(vec2(h, v))*edgeStrength;
    mediump float gray = smoothstep(threshold, threshold, 1.0-mag);
    if(gray<=0.5){
         gl_FragColor = vec4(vec3(gray), 1.0);
    }
    else{
        // gl_FragColor = vec4(0.9294, 0.9176, 0.8824, 1.0);
        gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    }
 }
);

GPUSobelEdgeFilter::GPUSobelEdgeFilter(float strength){
    // m_first_filter = new GPULuminanceFilter();
    m_first_filter = new GPUFilter();
    m_second_filter = new GPUFilter(g_sobeledge_fragment_shader);
    setExtraParameter(strength);
    setEdgeThreshold(0.95);
}

void GPUSobelEdgeFilter::setExtraParameter(float p){
    m_second_filter->setFloat("edgeStrength", p);
}

void GPUSobelEdgeFilter::setEdgeThreshold(float t){
    m_second_filter->setFloat("threshold", t);
}

void GPUSobelEdgeFilter::setInputFrameBuffer(GPUFrameBuffer *buffer, int location){
    GPUTwoPassFilter::setInputFrameBuffer(buffer, location);
    // 如果尺寸发生变化，重新设置尺寸
    if (m_frame_width!=buffer->m_width || m_frame_height!=buffer->m_height)
    {
        m_frame_width = buffer->m_width;
        m_frame_height = buffer->m_height;
        m_second_filter->setFloat("widthStep", 1.0/(m_frame_width-1));
        m_second_filter->setFloat("heightStep", 1.0/(m_frame_height-1));
    }
}


const static char* g_sobeldirection_fragment_shader = SHADER_STRING
(
 varying vec2 textureCoordinate;
 
 uniform sampler2D inputImageTexture;
 uniform mediump float edgeStrength;
 uniform mediump float widthStep;
 uniform mediump float heightStep;
 
 void main()
 {
    mediump float bottomLeftIntensity = texture2D(inputImageTexture, textureCoordinate+vec2(-widthStep, heightStep)).r;
    mediump float topRightIntensity = texture2D(inputImageTexture, textureCoordinate+vec2(widthStep, -heightStep)).r;
    mediump float topLeftIntensity = texture2D(inputImageTexture, textureCoordinate+vec2(-widthStep, -heightStep)).r;
    mediump float bottomRightIntensity = texture2D(inputImageTexture, textureCoordinate+vec2(widthStep, heightStep)).r;
    mediump float leftIntensity = texture2D(inputImageTexture, textureCoordinate+vec2(-widthStep, 0.0)).r;
    mediump float rightIntensity = texture2D(inputImageTexture, textureCoordinate+vec2(widthStep, 0.0)).r;
    mediump float bottomIntensity = texture2D(inputImageTexture, textureCoordinate+vec2(0.0, heightStep)).r;
    mediump float topIntensity = texture2D(inputImageTexture, textureCoordinate+vec2(0.0, -heightStep)).r;
     
	mediump vec2 gradientDirection;
	gradientDirection.x = -bottomLeftIntensity - 2.0 * leftIntensity - topLeftIntensity + bottomRightIntensity + 2.0 * rightIntensity + topRightIntensity;
	gradientDirection.y = -topLeftIntensity - 2.0 * topIntensity - topRightIntensity + bottomLeftIntensity + 2.0 * bottomIntensity + bottomRightIntensity;
    /*
	mediump float gradientMagnitude = length(gradientDirection);
	mediump vec2 normalizedDirection = normalize(gradientDirection);
	normalizedDirection = sign(normalizedDirection) * floor(abs(normalizedDirection) + 0.617316); // Offset by 1-sin(pi/8) to set to 0 if near axis, 1 if away
	normalizedDirection = (normalizedDirection + 1.0) * 0.5; // Place -1.0 - 1.0 within 0 - 1.0

	gl_FragColor = vec4(gradientMagnitude, normalizedDirection.x, normalizedDirection.y, 1.0);
     */
     mediump float tan = atan(gradientDirection.y/gradientDirection.x)*2.0/3.15;
     gl_FragColor = vec4(tan, tan, tan, 1.0);
 }
);
GPUSobelDirectionFilter::GPUSobelDirectionFilter(float strength):
GPUFilter(g_sobeldirection_fragment_shader){
    setExtraParameter(strength);
}

void GPUSobelDirectionFilter::setExtraParameter(float p){
    //setFloat("edgeStrength", p);
}

void GPUSobelDirectionFilter::setInputFrameBuffer(GPUFrameBuffer *buffer, int location){
    // 如果尺寸发生变化，重新设置尺寸
    if (m_frame_width!=buffer->m_width || m_frame_height!=buffer->m_height)
    {
        m_frame_width = buffer->m_width;
        m_frame_height = buffer->m_height;
        setFloat("widthStep", 1.0/(m_frame_width-1));
        setFloat("heightStep", 1.0/(m_frame_height-1));
    }
}

const static char* g_direction_fragment_shader = SHADER_STRING(
    varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;
    uniform mediump float widthStep;
    uniform mediump float heightStep;
    uniform mediump float radius;
    
    void main()
    {
        mediump float bottomLeftIntensity = texture2D(inputImageTexture, textureCoordinate+vec2(-widthStep*radius, heightStep*radius)).r;
        mediump float topRightIntensity = texture2D(inputImageTexture, textureCoordinate+vec2(widthStep*radius, -heightStep*radius)).r;
        mediump float topLeftIntensity = texture2D(inputImageTexture, textureCoordinate+vec2(-widthStep*radius, -heightStep*radius)).r;
        mediump float bottomRightIntensity = texture2D(inputImageTexture, textureCoordinate+vec2(widthStep*radius, heightStep*radius)).r;
        mediump float leftIntensity = texture2D(inputImageTexture, textureCoordinate+vec2(-widthStep*radius, 0.0)).r;
        mediump float rightIntensity = texture2D(inputImageTexture, textureCoordinate+vec2(widthStep*radius, 0.0)).r;
        mediump float bottomIntensity = texture2D(inputImageTexture, textureCoordinate+vec2(0.0, heightStep*radius)).r;
        mediump float topIntensity = texture2D(inputImageTexture, textureCoordinate+vec2(0.0, -heightStep*radius)).r;
        
        mediump float h = -topLeftIntensity - 2.0 * topIntensity - topRightIntensity + bottomLeftIntensity + 2.0 * bottomIntensity + bottomRightIntensity;
        mediump float v = -bottomLeftIntensity - 2.0 * leftIntensity - topLeftIntensity + bottomRightIntensity + 2.0 * rightIntensity + topRightIntensity;
        
        mediump float angle = atan(h/v)*2.0/3.15;
        if(angle<0.01){
            gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
        }
        else if(angle>0.01){
            gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        }
        else{
            // gl_FragColor = vec4(0.9294, 0.9176, 0.8824, 1.0);
            gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
        }
    }
);

GPUDirectionFilter::GPUDirectionFilter(): GPUFilter(g_direction_fragment_shader){
    setExtraParameter(2.0);
}

void GPUDirectionFilter::setExtraParameter(float p){
    setFloat("radius", p);
}

void GPUDirectionFilter::setFrameSize(uint32_t width, uint32_t height){
    m_frame_width = width;
    m_frame_height = height;
    
    setFloat("widthStep", 1.0/(m_frame_width-1));
    setFloat("heightStep", 1.0/(m_frame_height-1));
}

#pragma --mark "笔画细化"

const static char* g_stroke_fragment_shader = SHADER_STRING
(
 varying vec2 textureCoordinate;
 
 uniform sampler2D inputImageTexture;
 uniform mediump float widthStep;
 uniform mediump float heightStep;
 
 void main()
 {
    mediump float p1 = texture2D(inputImageTexture, textureCoordinate).r;
    mediump float p7 = texture2D(inputImageTexture, textureCoordinate+vec2(-widthStep, heightStep)).r;
    mediump float p3 = texture2D(inputImageTexture, textureCoordinate+vec2(widthStep, -heightStep)).r;
    mediump float p9 = texture2D(inputImageTexture, textureCoordinate+vec2(-widthStep, -heightStep)).r;
    mediump float p5 = texture2D(inputImageTexture, textureCoordinate+vec2(widthStep, heightStep)).r;
    mediump float p8 = texture2D(inputImageTexture, textureCoordinate+vec2(-widthStep, 0.0)).r;
    mediump float p4 = texture2D(inputImageTexture, textureCoordinate+vec2(widthStep, 0.0)).r;
    mediump float p6 = texture2D(inputImageTexture, textureCoordinate+vec2(0.0, heightStep)).r;
    mediump float p2 = texture2D(inputImageTexture, textureCoordinate+vec2(0.0, -heightStep)).r;
     
     if ((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) >= 2.0 && (p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) <= 6.0)
     {
         int ap = 0;
         if (p2 == 1.0 && p3 == 0.0) ++ap;
         if (p3 == 1.0 && p4 == 0.0) ++ap;
         if (p4 == 1.0 && p5 == 0.0) ++ap;
         if (p5 == 1.0 && p6 == 0.0) ++ap;
         if (p6 == 1.0 && p7 == 0.0) ++ap;
         if (p7 == 1.0 && p8 == 0.0) ++ap;
         if (p8 == 1.0 && p9 == 0.0) ++ap;
         if (p9 == 1.0 && p2 == 0.0) ++ap;
         
         if (ap == 1 && p2 + p4 + p6 >= 1.0 && p4 + p6 + p8 >= 1.0)
         {
             gl_FragColor = vec4(1.0);
         }
         else{
             gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
         }
     }
     else{
         gl_FragColor = vec4(1.0);
     }
 }
);

const static char* g_spine_fragment_shader = SHADER_STRING
(
 varying vec2 textureCoordinate;
 
 uniform sampler2D inputImageTexture;
 uniform mediump float widthStep;
 uniform mediump float heightStep;
 
 void main()
 {
    mediump float p1 = texture2D(inputImageTexture, textureCoordinate).r;
    mediump float p7 = texture2D(inputImageTexture, textureCoordinate+vec2(-widthStep, heightStep)).r;
    mediump float p3 = texture2D(inputImageTexture, textureCoordinate+vec2(widthStep, -heightStep)).r;
    mediump float p9 = texture2D(inputImageTexture, textureCoordinate+vec2(-widthStep, -heightStep)).r;
    mediump float p5 = texture2D(inputImageTexture, textureCoordinate+vec2(widthStep, heightStep)).r;
    mediump float p8 = texture2D(inputImageTexture, textureCoordinate+vec2(-widthStep, 0.0)).r;
    mediump float p4 = texture2D(inputImageTexture, textureCoordinate+vec2(widthStep, 0.0)).r;
    mediump float p6 = texture2D(inputImageTexture, textureCoordinate+vec2(0.0, heightStep)).r;
    mediump float p2 = texture2D(inputImageTexture, textureCoordinate+vec2(0.0, -heightStep)).r;
     
     if (p1<0.1 && (p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) < 5.0 )
     {
         gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
     }
     else{
         gl_FragColor = vec4(1.0);
     }
 }
);

GPUStrokeFilter::GPUStrokeFilter(int num): GPUFilter(g_spine_fragment_shader){
    setIterations(num);
}

void GPUStrokeFilter::render(){
    for (int i=0; i<m_iternum; i++) {
        GPUFilter::render();
        setInputFrameBuffer(m_outbuffer);
    }
    
    unlockInputFrameBuffers();
}

// 迭代次数
void GPUStrokeFilter::setIterations(int num){
    m_iternum = num;
}

void GPUStrokeFilter::setFrameSize(uint32_t width, uint32_t height){
    m_frame_width = width;
    m_frame_height = height;
    
    setFloat("widthStep", 1.0/(m_frame_width-1));
    setFloat("heightStep", 1.0/(m_frame_height-1));
}

/**
 * file :	GPUZoomFilter.cpp
 * author :	Rex
 * email : rex@labjk.com
 * create :	2016-07-12 20:33
 * func : 
 * history:
 */

#include "GPUZoomFilter.h"

// ios需要输出bgra格式
static const char* g_bgra_fragment_shader = SHADER_STRING(
    varying vec2 textureCoordinate;
    //uniform samplerExternalOES inputImageTexture;
    uniform sampler2D inputImageTexture;
                                                         
    void main()
    {
        gl_FragColor = texture2D(inputImageTexture, textureCoordinate).bgra;
    }
);

GPUZoomFilter::GPUZoomFilter(gpu_fill_mode_t mode)
{
    setFillMode(mode);
}

void GPUZoomFilter::render(){
    GPUFilter::render();
    glFinish();
}

void GPUZoomFilter::setOutputFormat(gpu_pixel_format_t format){
	const char *fragment = (format==GPU_BGRA) ? g_bgra_fragment_shader : g_fragment_shader;
	changeShader(fragment, g_vertext_shader[0]);
}

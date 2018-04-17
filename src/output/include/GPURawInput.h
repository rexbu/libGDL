/**
 * file :	GPURawInput.h
 * author :	Rex
 * email : rex@labjk.com
 * create :	2016-04-27 21:10
 * func : 
 * history:
 */

#ifndef	__GPU_RAWINPUT_H_
#define	__GPU_RAWINPUT_H_
#include "GPUContext.h"
#include "GPUOutput.h"
#include "GPUFilter.h"
#include "GPUYUVFilter.h"

class GPURawInput: public GPUOutput{
public:
    GPURawInput();
    GPURawInput(gpu_pixel_format_t format);
    GPURawInput(int width, int height, gpu_pixel_format_t format = GPU_RGBA);
    ~GPURawInput();
    
    // m_width和m_height是输出尺寸，和此处width、height可能为旋转关系，注意区别
    void uploadBytes(GLubyte* bytes, int width, int height, gpu_pixel_format_t in_type = GPU_NV21);
    void uploadBytes(GLubyte* bytes);
    
    gpu_pixel_format_t getFormat(){
        return m_in_format;
    }
    /**
     * 如果为nv21，设置美白
     */
    void setStrength(float level);
protected:
    GPUFilter*          m_filter;
    
    gpu_pixel_format_t    m_in_format;
    int m_width;
    int m_height;
};

#endif

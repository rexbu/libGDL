/**
 * file :	GPUTextureInput.h
 * author :	Rex
 * email : rex@labjk.com
 * create :	2016-12-08 15:39
 * func : 
 * history:
 */

#ifndef	__GPUTEXTUREINPUT_H_
#define	__GPUTEXTUREINPUT_H_

#include "GPUContext.h"
#include "GPUOutput.h"
#include "GPUFilter.h"

class GPUTextureInput: public GPUFilter {
public:
    GPUTextureInput();
    GPUTextureInput(int width, int height, int texture_type = GPU_TEXTURE_OES);
    void processTexture();
    void processTexture(GLuint texture);


/// void render();
    gpu_size_t getSize(){
        gpu_size_t size = {m_out_width, m_out_height};
        return size;
    }
    // 设置视频输出尺寸，可能和输入不一致，为旋转关系
    // void setOutputSize(int width, int height){
    //     m_width = width;
    //     m_height = height;
    // }
    virtual void render();                              /// 重载父类函数，渲染流程，解锁输入
protected:
    GLuint  m_texture;
    int     m_texture_type;
/// int m_width, m_height;
};

#endif

/**
 * file :	GPUBuffer.h
 * author :	Rex
 * email : rex@labjk.com
 * create :	2016-04-27 19:12
 * func : 
 * history:
 */

#ifndef	__GPU_BUFFER_H_
#define	__GPU_BUFFER_H_
#include <vector>
#include "GL.h"
#include "GPUContext.h"

using namespace std;

typedef struct gpu_frame_option_t{
    gpu_texture_type_t texture_type;    // 纹理类别
    GLenum texture_dim;     // 纹理维度，GL_TEXTURE_2D或者GL_TEXTURE_3D
    GLenum min_filter;      // 缩放差值方法
    GLenum mag_filter;      // 缩放差值方法
    GLenum wrap_s;          // s方向边缘填充方式
    GLenum wrap_t;          // t方向边缘填充方式
    GLenum wrap_r;          // r方向边缘填充方式
    GLenum color_format;    // 纹理内存存储格式，未确定大小纹理格式GL_RGBA GL_ALPHA，确定大小GL_R8 RGBA8UI
    GLenum format;          // 纹理格式，如GL_RED GL_RGBA GL_ALPHA
    GLenum type;            // 输入像素数据类型，如GL_UNSIGNED_BYTE
    GLenum depth_format;    // 深度纹理位数
    GLenum attachment;      // 缓冲区附着类型
}gpu_frame_option_t;

class GPUFrameBuffer{
public:
    /// 2D纹理
    GPUFrameBuffer(gpu_size_t size, bool only_texture = true);
    GPUFrameBuffer(int width, int height, bool only_texture = true);
    GPUFrameBuffer(int width, int height, gpu_frame_option_t* option, bool only_texture = true);
    GPUFrameBuffer(int width, int height, int depth, gpu_frame_option_t* option, bool only_texture = true);
    GPUFrameBuffer(int width, int height, GLuint texture, gpu_frame_option_t* option=NULL);
    
    /// 默认构造函数只用于GPUIOSFrameBuffer的初始化，不能用于其他地方
    GPUFrameBuffer(){
        m_width = m_height = m_texture = m_framebuffer = m_referencecount = 0;
        m_is_reference = true;
    }
    
    ~GPUFrameBuffer(){
        destroy();
    }
    /// 生成纹理
    virtual void generateTexture();
    static int generateTexture(int width, int height, int depth, gpu_frame_option_t* option);
    /// 生成FBO
    virtual void generateFrameBuffer();
    /// 向纹理填充像素
    void setPixels(void* data);
    uint8_t* getPixels(uint8_t* data=NULL);
    /// 绑定fbo
    void activeBuffer(int layer=0);
    /// 设置活动纹理单元
    void activeTexture(GLuint texture_unit);
    /// 设置不活动
    void unactive();
    /// 加锁
    bool lock();
    /// 判断是否被占用
    bool idle();
    /// 减锁
    bool unlock();
    // 不再加锁，也就是参加统计计数
    void disableReference();
    // 释放锁
    void release();
    
    /// 默认配置
    static gpu_frame_option_t defaultFrameOption();
    static gpu_frame_option_t* nearestFrameOption();
    static gpu_frame_option_t* uintFrameOption();
    static gpu_frame_option_t* floatFrameOption();
protected:

    void init(int width, int height, int depth, gpu_frame_option_t* option, bool only_texture = true);
    void destroy();
    
public:
    gpu_frame_option_t m_option;
    GLuint      m_framebuffer;
    GLuint      m_texture;
    GLuint      m_renderbuffer;
    bool        m_outtexture;   // 是否是外部传入texture，外部传入destroy不释放，谁创建谁管理
    int         m_width;
    int         m_height;
    int         m_depth;
    uint8_t*    m_rgba;
    int         m_current_layer;    // 3D纹理当前的层

    // 被使用次数
    int         m_referencecount;
    bool        m_is_reference;
};

#define GPU_FRAMEBUFFER_NUM 1024
class GPUBufferCache{
public:
    static GPUBufferCache* shareInstance();
    static void destroyInstance();
    GPUBufferCache();
    
    GPUFrameBuffer* getFrameBuffer(int width, int height, bool only_texture = true);
    GPUFrameBuffer* getFrameBuffer(gpu_size_t size, bool only_texture = true);
    GPUFrameBuffer* getFrameBuffer(gpu_size_t size, gpu_frame_option_t* option, bool only_texture = true);
    GPUFrameBuffer* getFrameBuffer(int width, int height, gpu_frame_option_t* option, bool only_texture = true);
    GPUFrameBuffer* getFrameBuffer(int width, int height, int depth, gpu_frame_option_t* option, bool only_texture = true);
    
    // 新建FrameBuffer，子函数可能复写，在IOS中创建方式不一样
    virtual GPUFrameBuffer* newFrameBuffer(int width, int height, int depth, gpu_frame_option_t* option, bool only_texture=true);
    bool setFrameBuffer(GPUFrameBuffer* frame);
    // 清空framebuffer
    void clear();
    // 将不用的framebuffer释放
    void purge();

    vector<GPUFrameBuffer*> m_bufferlist;
protected:
    static GPUBufferCache* m_instance;
};

#endif

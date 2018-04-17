/**
 * file :	GPUCNN.h
 * author :	Rex
 * create :	2017-07-27 18:39
 * func : 
 * history:
 */

#ifndef	__GPUCNN_H_
#define	__GPUCNN_H_

#include "GPU.h"

/// 池化方式
typedef enum {
    CONV_POOL_NULL,
    CONV_POOL_MAX,      // 按照最大值进行池化
    CONV_POOL_AVERAGE   // 按照平均值进行池化
}conv_pool_t;

/// 激活函数
typedef enum{
    CONV_ACTIVE_NULL = 0,
    CONV_ACTIVE_RELU,
    CONV_ACTIVE_SIGMOID,
    CONV_ACTIVE_TANH
}conv_active_t;

/// 卷积层类型
typedef enum{
    CONV_LAYER_NORMAL=0,
    CONV_LAYER_FULL
}conv_type_t;

typedef enum {
    CONV_PADDING_NULL = 0,
    CONV_PADDING_ZERO,
    CONV_PADDING_REFLECT
}conv_pad_t;
/**
 * 卷积层基类
 */
class GPULayerBase: public GPUFilter{
public:
    /**
     * @channel_count 输入通道数，也即是上一卷积层的卷积核数量
     * @kernel_count 卷积核数量，也即是当前卷积层的输出通道数
     */
    GPULayerBase(int kernel_count, const char* name = "GPULayerBase");
    virtual ~GPULayerBase(){
        if (m_outbuffer!=NULL) {
            delete m_outbuffer;
        }
    }
    
    virtual void newFrame();
    // void activeOutFrameBuffer();
    virtual void setInputFrameBuffer(GPUFrameBuffer* buffer, int location=0);
    virtual void setFrameSize(uint32_t width, uint32_t height);
    
    static GPUFrameBuffer* floatFrameBuffer(float* v, int w, int h);
    
    virtual int addTarget(GPULayerBase *target, int location = 0){
        if (location == 0) {
            dynamic_cast<GPULayerBase*>(target)->m_last_layer = this;
            // 下一层的通道数为当前层的卷积数
            dynamic_cast<GPULayerBase*>(target)->m_channel_count = m_kernel_count;
        }
        
        return GPUOutput::addTarget(target, location);
    }
    
    virtual int addTarget(GPUInput *target, int location = 0){
        return GPUOutput::addTarget(target, location);
    }
    
    /**
     * 获取输出通道排列，x方向和y方向上的通道数
     */
    gpu_size_t getOutChannelCount(){
        gpu_size_t size = {(uint32_t)m_outx_count, (uint32_t)m_outy_count};
        return size;
    }
    
    static void print(GPUFrameBuffer* buffer, bool rgba = false, const char* name=NULL);
    void setTensor(const char* name);
    virtual void diff();
    float*  m_out;
public:
    int                     m_channel_count;
    int                     m_kernel_count;
    int                     m_outx_count;     // 水平方向输出通道数
    int                     m_outy_count;     // 垂直方向输出通道数
    GPULayerBase*           m_last_layer;
    
    float*                  m_values;   // 用于读取tensorflow结果进行diff
};

#endif

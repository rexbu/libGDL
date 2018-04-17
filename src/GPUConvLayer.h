/**
 * file :	GPUConvLayer.h
 * author :	Rex
 * create :	2017-11-25 18:32
 * func :   支持Padding的卷积层，默认使用2x2池化，如果使用更加复杂的池化，请使用GPUPoolLayer
 * history:
 */

#ifndef	__GPUCONVLAYER_H_
#define	__GPUCONVLAYER_H_

#include "GPU.h"
#include "GPUCNN.h"

class GPUConvLayer: public GPULayerBase{
public:
    /**
     * @kernel_size     卷积核大小
     * @channel_size    通道数，也即上一层卷积的卷积核数
     * @kernel_count    卷积核数量
     * @active_type     激活函数
     */
    GPUConvLayer(int kernel_size, int kernel_count, int next_padding=0, conv_active_t active_type=CONV_ACTIVE_RELU, const char* name = "ConvLayer");
    ~GPUConvLayer();
    /**
     * 设置卷积核，conv_kernel有kernel_size^2*channel_size*kernel_count个float
     * @param conv_kernel    卷积核数据
     */
    void setConvKernel(float* conv_kernel, int size);
    
    /**
     * 设置偏移, 元素数量为kernel_count
     */
    void setBias(float* bias, int size);
    
    virtual void render();
    virtual void activeOutFrameBuffer();
    virtual void setFrameSize(uint32_t width, uint32_t height);
    void coordinateMap();
    GPUFrameBuffer* mapTexture(int channel_width, int channel_height);
    
    virtual void diff();
    void debug();
public:
    int             m_kernel_size;
    int             m_padding_size;
    int             m_next_padding;
    conv_active_t   m_active_type;
    
    GPUFrameBuffer* m_map_buffer;   // 存储输出像素在原通道及卷积中的偏移
    GPUFrameBuffer* m_kernel_buffer;    // 存储卷积核
    float*          m_kernel;
    float*          m_map_values;
    
    GLuint              m_kernel_map;
    GPUVertexBuffer*    m_kernel_mapbuffer;
};
#endif

/**
 * file :	GPUFullConnLayer.h
 * author :	Rex
 * create :	2017-12-05 11:39
 * func :   全连接层
 * history:
 */

#ifndef	__GPUFULLCONNLAYER_H_
#define	__GPUFULLCONNLAYER_H_

#include "GPU.h"
#include "GPUCNN.h"

class GPUFullConnLayer: public GPULayerBase{
public:
    /**
     * @out_dim 输出向量维数
     */
    GPUFullConnLayer(int out_dim, conv_active_t active = CONV_ACTIVE_RELU,const char* name="FullConnLayer");
    ~GPUFullConnLayer();
    
    /** 全连接层系数矩阵和bias
     * @weight: 系数矩阵
     * @bais:   偏移向量
     * @w:      矩阵宽，如果池化为1x1则等于通道数，否则等于每个通道的大小*通道数
     * @h:      矩阵高，等于卷积核数
     */
    void setWeights(float* weight, float* bais, int w, int h);
    
    virtual void setFrameSize(uint32_t width, uint32_t height);
    virtual void diff();
    
protected:
    void generateInputBuffer();
    
    int                 m_out_dim;
    GLint               m_weight_texture;
    GPUFrameBuffer*     m_weight_buffer;    // 系数矩阵
    float*              m_output_vector;
    float*              m_weights;
};
#endif

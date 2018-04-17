/**
 * file :	GPUPadding.h
 * author :	Rex
 * create :	2018-01-05 17:43
 * func : 
 * history:
 */

#ifndef	__GPUPADDING_H_
#define	__GPUPADDING_H_

#include "GPUCNN.h"

class GPUPadding: public GPULayerBase{
public:
    /**
     * @padding_size: 分别为左上右下
     */
    GPUPadding(int channel_count, int padding_size, conv_pad_t padding_type=CONV_PADDING_ZERO);
    
    virtual void render();
    virtual void activeOutFrameBuffer();
    
protected:
    // 填充0的padding顶点计算
    void zeroVertexCoor();
    // 镜像padding顶点计算
    void reflectVertexCoor();
    
    int             m_padding_size;
    conv_pad_t      m_padding_type;
    
    GPUFrameBuffer* m_map_buffer;
};

#endif

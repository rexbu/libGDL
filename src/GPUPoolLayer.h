/**
 * file :	GPUPoolLayer.h
 * author :	Rex
 * create :	2017-12-05 14:36
 * func : 
 * history:
 */

#ifndef	__GPUPOOLLAYER_H_
#define	__GPUPOOLLAYER_H_

#include "GPU.h"
#include "GPUCNN.h"

class GPUPoolLayer: public GPULayerBase{
public:
    GPUPoolLayer(int channel_count, int pool_size=2, conv_pool_t pool_type=CONV_POOL_MAX,int next_padding=0, const char* name = "PoolLayer");
    ~GPUPoolLayer();
    
    virtual void render();
    virtual void activeOutFrameBuffer();
    virtual void setFrameSize(uint32_t width, uint32_t height);
    
    virtual void diff();
protected:
    void coordinateMap();
    GPUFrameBuffer* mapTexture(int channel_width, int channel_height);
    
    int         m_pool_size;
    int         m_next_padding;
    conv_pool_t m_pool_type;
    
    GPUFrameBuffer* m_map_buffer;
    
    GLuint              m_channel_map;
    GPUVertexBuffer*    m_channel_buffer;
};

#endif

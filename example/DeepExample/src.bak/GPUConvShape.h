/**
 * file :	GPUConvShape.h
 * author :	Rex
 * create :	2017-07-27 18:48
 * func :   卷积输入
 * history:
 */

#ifndef	__GPUCONVSHAPE_H_
#define	__GPUCONVSHAPE_H_

#include "bs.h"
#include "GPU.h"
#include "GDLGlobal.h"

// texture是用nearest
class GPUConvShape: public GPULayerBase{
public:
    /**
     * @normal_size 归一化尺寸
     * @channel_size 通道数，1-4分别对应rgba，相当于GPULayerBase的卷积核数
     * @pending_size RGB卷积层的pending尺寸
     */
    GPUConvShape(int normal_size, int channel_count=3, int next_padding = 0);
    ~GPUConvShape(){
        if (m_outcoor_buffer!=NULL) {
            delete m_outcoor_buffer;
        }
    }
    
    virtual void render();
    virtual void newFrame();
    virtual void calAspectRatio();
    virtual void activeOutFrameBuffer();
    
    void print();
protected:
    int         m_normal_size;
    int         m_channel_count;
    int         m_next_padding;
    int         m_input_texture;
    
    GLint               m_out_coordinate;
    GPUVertexBuffer*    m_outcoor_buffer;
    vector<float>       m_out_coordinates;
};
#endif

/**
 * file :	ConvFrameBuffer.h
 * author :	Rex
 * create :	2017-12-20 19:46
 * func : 
 * history:
 */

#ifndef	__CONVFRAMEBUFFER_H_
#define	__CONVFRAMEBUFFER_H_

#include "GPU.h"

class ConvFrameBuffer: public GPUFrameBuffer{
public:
    /**
     * @channel_width 单个通道的宽
     * @channel_height 单个通道的高
     * @padding_size 每个通道需要添加的padding尺寸
     * @x_count 水平方向通道的个数
     * @y_count 垂直方向通道个数
     * @count 通道总数，默认为x_count*y_count
     */
    ConvFrameBuffer(int channel_width, int channel_height, int padding_size, int x_count, int y_count, int count=0):
    m_channel_width(channel_width),
    m_channel_height(channel_height),
    m_padding_size(padding_size),
    m_x_count(x_count),
    m_y_count(y_count),
    m_channel_count(count),
    // 每个通道(channel_width+padding_size)*(channel_height+padding_size)大小，在最右添加一列0，最下添加一行0，防止GL_CLAMP_TO_EDGE引起的边缘计算错误
    GPUFrameBuffer((channel_width+2*padding_size)*x_count, (channel_height+2*padding_size)*y_count, GPUFrameBuffer::uintFrameOption(), false){
        // 如果传入了count的值，表示x_count*y_count大于总通道数
        if (m_channel_count==0) {
            m_channel_count = x_count*y_count;
        }
    }
    
    /// 顶点数量
//    inline uint32_t vertexCount(){
//        return (int32_t)m_channel_count*6;
//    }
    // 由输出framebuffer调用
    float* poolVertices();
    // 由输入framebuffer调用，也即是卷积的输出调用，无padding
    float* poolCoordinates();
    /*
     * 卷积用顶点和坐标点。输入带padding，右侧的padding不进行卷积计算
     * *xx*  卷积计算后 xx**
     * *xx*           xx**
     * 顶点计算去掉右侧padding进行的无用的卷积计算
     */
    // 应该由当前filter的输出fbo调用
    float* convVertices();
    // 应该由当前filter的输出fbo调用，用于mapTexture的坐标查找
    float* convCoordinates();
    /*
     * 所有pading框也进入渲染，周围共有9个padding框
     * reflectVertices应该由当前filter的outbuffer调用，用于计算顶点坐标
     */
    float* reflectVertices();
    // reflectCoordinates应该由输入fbo调用，用于计算输入纹理坐标，参数padding为当前filter要添加的padding，和m_padding_size可能不一样
    float* reflectCoordinates(int padding);
    uint16_t* vertexIndexs(){
        return &m_indexs[0];
    }
    uint32_t vertexCount(){
        return (uint32_t)m_indexs.size();
    }
    
    void triangles_copy(vector<float>& v, float* p);
public:
    int             m_channel_width;
    int             m_channel_height;
    int             m_padding_size;
    int             m_x_count;
    int             m_y_count;
    int             m_channel_count;
    
    vector<float>   m_vertices;
    vector<float>   m_coordinates;
    vector<uint16_t>m_indexs;
};

#endif

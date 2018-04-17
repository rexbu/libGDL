/**
 * file :	GPUColorSpread.h
 * author :	Rex
 * create :	2018-01-25 17:23
 * func : 
 * history:
 */

#ifndef	__GPUCOLORSPREAD_H_
#define	__GPUCOLORSPREAD_H_

#include "bs.h"
#include "GPU.h"
#include "GDLGlobal.h"

// texture是用nearest
class GPUColorSpread: public GPULayerBase{
public:
    /**
     * @normal_size 归一化尺寸
     * @channel_size 通道数，1-4分别对应rgba，相当于GPULayerBase的卷积核数
     * @pending_size RGB卷积层的pending尺寸
     */
    GPUColorSpread(int channel_count=3);
    
    virtual void activeOutFrameBuffer();
    
    void print();
protected:
    int         m_channel_count;
    int         m_input_texture;
    
    short               m_indexs[18];
};

#endif

/**
 * file :	GPUAdaIn.h
 * author :	Rex
 * create :	2018-02-09 12:56
 * func : 
 * history:
 */

#ifndef	__GPUADAIN_H_
#define	__GPUADAIN_H_

#include "GPUCNN.h"

class GPUAdaIn: public GPULayerBase{
public:
    GPUAdaIn();
    ~GPUAdaIn();
    
    void setStyle(float* mean, float* var, int width, int height);
    // virtual void newFrame();
    virtual void setFrameSize(uint32_t width, uint32_t height);
    virtual void activeOutFrameBuffer();
    
    virtual void diff();
protected:
    GPUFrameBuffer*     m_mean_buffer;
    GPUFrameBuffer*     m_var_buffer;
};

#endif

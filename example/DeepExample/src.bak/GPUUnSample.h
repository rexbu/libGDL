/**
 * file :	GPUUnSample.h
 * author :	Rex
 * create :	2018-02-11 00:47
 * func : 
 * history:
 */

#ifndef	__GPUUNSAMPLE_H_
#define	__GPUUNSAMPLE_H_

#include "GPUCNN.h"
class GPUUnSample: public GPULayerBase{
public:
    
    GPUUnSample(int size = 2);
    virtual void activeOutFrameBuffer();
    
protected:
    int     m_unsample_size;
};

#endif

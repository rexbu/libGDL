/**
 * file :	GPUTest.h
 * author :	Rex
 * create :	2017-07-27 18:36
 * func : 
 * history:
 */

#ifndef	__GPUTEST_H_
#define	__GPUTEST_H_

#include "GPUFilter.h"
#include "GPURawOutput.h"

class GPUTestFilter: public GPUFilter{
public:
    GPUTestFilter();
    //virtual void render();
    virtual void newFrame();
    
    GPURawOutput    m_output;
    unsigned char*  m_buffer;
    
    int w, h;
};

#endif

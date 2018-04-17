/**
 * file :	GPUGroupFilter.h
 * author :	Rex
 * email : rex@labjk.com
 * create :	2016-08-06 21:57
 * func : 
 * history:
 */

#ifndef	__GPUGROUPFILTER_H_
#define	__GPUGROUPFILTER_H_

#include "GPUFilter.h"

class GPUGroupFilter: public GPUFilter{
public:
    GPUGroupFilter(const char* name = NULL);
    virtual void newFrame();
    virtual void setInputFrameBuffer(GPUFrameBuffer* buffer, int location=0);
    
    inline void setFirstFilter(GPUFilter* f){
        m_input = f;
    }
    inline void setLastFilter(GPUFilter* f){
        m_output = f;
    }
    
    inline GPUFilter* getFirstFilter(){ return m_input; }
    inline GPUFilter* getLastFilter(){ return m_output; }

    GPUFilter*  m_input;    // Group内第一个filter
    GPUFilter*  m_output;   // Group内最后一个filter
};

#endif

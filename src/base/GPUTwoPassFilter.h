/**
 * file :	GPUTwoPassFilter.h
 * author :	Rex
 * email : rex@labjk.com
 * create :	2016-05-05 19:49
 * func : 
 * history:
 */

#ifndef	__GPUTWOPASSFILTER_H_
#define	__GPUTWOPASSFILTER_H_
#include "GPUFilter.h"

class GPUTwoPassFilter: public GPUFilter{
public:
    GPUTwoPassFilter();
    GPUTwoPassFilter(const char* first_fragment, const char* second_fragment);
    GPUTwoPassFilter(const char* first_vertext, const char* first_fragment, const char* second_vertext, const char* second_fragment);
    
    virtual ~GPUTwoPassFilter(){
        delete m_first_filter;
        m_first_filter = NULL;
        delete m_second_filter;
        m_second_filter = NULL;
    }
    
    virtual void init_filter();
    void render();
    virtual void setInputFrameBuffer(GPUFrameBuffer* buffer, int location=0);
    virtual void setOutputRotation(gpu_rotation_t rotation){
        m_rotation = rotation;
        m_first_filter->setOutputRotation(rotation);
    }
    
    // 修改shader
    void changeShader(const char* first_fragment, const char* second_fragment);
protected:
    GPUFilter*  m_first_filter;
    GPUFilter*  m_second_filter;
};
#endif

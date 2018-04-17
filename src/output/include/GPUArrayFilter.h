/**
 * file :	GPUArrayFilter.h
 * author :	Rex
 * create :	2017-11-26 13:17
 * func : 
 * history:
 */

#ifndef	__GPUARRAYFILTER_H_
#define	__GPUARRAYFILTER_H_

#include "GPUFilter.h"

class GPUArrayFilter: public GPUFilter{
public:
    GPUArrayFilter(bool compile = true, const char* name = NULL); // compile，是否需要编译
    GPUArrayFilter(const char* fragment, int inputs=1, const char* name=NULL);
    GPUArrayFilter(const char* vertext, const char* fragment, int inputs=1);
    
    virtual void initShader();
    virtual void render();
    
};

#endif

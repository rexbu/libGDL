/**
 * file :	GPUTriangleFilter.h
 * author :	Rex
 * email : rex@labjk.com
 * create :	2017-02-25 23:32
 * func : 
 * history:
 */

#ifndef	__GPUTRIANGLEFILTER_H_
#define	__GPUTRIANGLEFILTER_H_

#include "GPUFilter.h"

class GPUTriangleFilter: public GPUFilter{
public:
    GPUTriangleFilter(const char* fragment, int inputs=1, int vertex=4);
    void setVertex(float* vertex, int count);
    
    virtual void render();
};

#endif

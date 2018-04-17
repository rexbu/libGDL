/**
 * file :	GDLStyle.h
 * author :	Rex
 * create :	2018-01-28 00:07
 * func : 
 * history:
 */

#ifndef	__GDLSTYLE_H_
#define	__GDLSTYLE_H_

#include "GDLModel.h"
#include "GPUCNN.h"
#include "GDLSession.h"
#include "GPUAdaIn.h"

class GDLStyle: public GDLSession{
public:
    GDLStyle(const char* model);
    ~GDLStyle();
    
    void style(const char* style_path);
    void style(float* style);
    void encoder();
    void adain();
    void decoder();
    void saveStyle(const char* path);
    
    void loadStyleImage(const char* path);
    void loadStyleModel(const char* path);
    
protected:
    GPUConvLayer*   m_encoder_4_1;
    GPUMean*        m_mean;
    GPUVariance*    m_var;
    GPUAdaIn*       m_ada;
};

#endif

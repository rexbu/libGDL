/**
 * file :	GPUYUVFilter.h
 * author :	Rex
 * email : rex@labjk.com
 * create :	2016-05-03 15:54
 * func : 
 * history:
 */

#ifndef	__GPUYUVFILTER_H_
#define	__GPUYUVFILTER_H_

#include "GPUFilter.h"

/// NV21->YUV
class GPUNV21ToYUVFilter: public GPUFilter{
public:
    GPUNV21ToYUVFilter();
    GPUNV21ToYUVFilter(const char* fragment);
    GPUNV21ToYUVFilter(float s, int cc);
    
    void setIdeal(float ideal[4]);
    void setThreshold(float thresh[4]);
    void setStrength(float s);
protected:
    GLfloat m_idealpnts[8];
    GLfloat m_threshold[4];
    float 	m_strength;
    
    GLfloat m_rgb2yuv[9];
    GLfloat m_off2yuv[3];
};

/// NV12->YUV
class GPUNV12ToYUVFilter: public GPUNV21ToYUVFilter{
public:
    GPUNV12ToYUVFilter();
    GPUNV12ToYUVFilter(float s, int cc);
};

/// NV21->RGB
class GPUNV21ToRGBFilter: public GPUFilter{
public:
    GPUNV21ToRGBFilter();
protected:
    GLint m_conversion_matrix;
};
// NV12->RGB
class GPUNV12ToRGBFilter: public GPUFilter{
public:
    GPUNV12ToRGBFilter();
protected:
    GLint m_conversion_matrix;
};
// I420->RGB
class GPUI420ToRGBFilter: public GPUFilter{
public:
    GPUI420ToRGBFilter();
protected:
    GLint m_conversion_matrix;
};
// YUV->RGB
class GPUYUVToRGBFilter: public GPUFilter{
public:
    GPUYUVToRGBFilter();
};

class GPURGBToYUVFilter: public GPUFilter{
public:
    GPURGBToYUVFilter();
    GPURGBToYUVFilter(float s, int cc);

    void setIdeal(float ideal[4]);
    void setThreshold(float thresh[4]);
    void setStrength(float s);
protected:
    GLfloat m_idealpnts[8];
    GLfloat m_threshold[4];
    float 	m_strength;
    
    GLfloat m_rgb2yuv[9];
    GLfloat m_off2yuv[3];
};
#endif

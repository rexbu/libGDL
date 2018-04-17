/**
 * file :	GPU.h
 * author :	Rex 
 * email : rex@labjk.com
 * create :	2016-04-28 22:29
 * func : 
 * history:
 */

#ifndef	__GPU_H_
#define	__GPU_H_

#include "GL.h"
#include "GPUContext.h"     // glContext
#include "GPUProgram.h"     // glProgram
#include "GPUInput.h"       // 接受流
#include "GPUOutput.h"      // 输出流
#include "GPUFrameBuffer.h"
#include "GPUPixelBuffer.h"
#include "GPUFilter.h"
#ifdef __ANDROID__
#include "GPUTextureInput.h"
#endif
#include "GPURawInput.h"            // byte/texture视频输出源
#include "GPUTwoPassFilter.h"       // x、y方向filter
#include "GPUGroupFilter.h"         // 多filter组合
#include "GPUYUVFilter.h"           // yuv、rgb转换相关
#include "GPUView.h"                // 输出到显存
#include "GPURawOutput.h"           // 获取处理后byte流
#include "GPUOutputFilter.h"
#include "GPUZoomFilter.h"          // 缩放filter
#include "GPUYUV420Filter.h"        // yuv420、nv21相关转换
#include "GPUBlendFilter.h"
#include "GPULineFilter.h"

#include "GPUMedianFilter.h"        // 中值滤波
#include "GPUGaussianBlurFilter.h"  // 高斯滤波
#include "GPULuminanceFilter.h"     // 灰度
#include "GPUSobelEdgeFilter.h"     // Sobel边缘检测
#include "GPUBilateralBlurFilter.h" // 平滑滤波
#include "GPUColorFilter.h"

#include "GPUBezierFilter.h"        // 贝塞尔曲线
#include "GPUPaintFilter.h"

#include "GPUBlendImageFilter.h"
#include "GPUBlend2Filter.h"
#include "GPUPixellateFilter.h"
#include "GPUPixellatePositionFilter.h"

#if !defined(MIN)
#define MIN(A,B)	({ __typeof__(A) __a = (A); __typeof__(B) __b = (B); __a < __b ? __a : __b; })
#endif

#if !defined(MAX)
#define MAX(A,B)	({ __typeof__(A) __a = (A); __typeof__(B) __b = (B); __a < __b ? __b : __a; })
#endif

#if !defined(ABS)
#define ABS(A)	({ __typeof__(A) __a = (A); __a < 0 ? -__a : __a; })
#endif

#define DELETE_SET_NULL(ptr, is_array) \
    if ((ptr) != NULL) { \
        if (is_array) { \
            delete []ptr; \
        } else { \
            delete ptr; \
        } \
    } \
    ptr = NULL;

#endif

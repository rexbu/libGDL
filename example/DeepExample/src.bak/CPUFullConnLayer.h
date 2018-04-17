/**
 * file :	CPUFullConnLayer.h
 * author :	Rex
 * create :	2018-01-01 13:56
 * func : 
 * history:
 */

#ifndef	__CPUFULLCONNLAYER_H_
#define	__CPUFULLCONNLAYER_H_

#include <map>
#include "bs.h"
#include "GPUCNN.h"
#include "ThreadPool.h"

class CPUFullConnLayer:public GPULayerBase{
public:
    CPUFullConnLayer(float* wei, float* bias, int w, int h);
    ~CPUFullConnLayer();
    
    virtual void diff();
    /** 全连接层系数矩阵和bias
     * @weight: 系数矩阵
     * @bais:   偏移向量
     * @w:      矩阵宽，如果池化为1x1则等于通道数，否则等于每个通道的大小*通道数
     * @h:      矩阵高，等于卷积核数
     */
    void setWeights(float* wei, float* bias, int w, int h);
    
    inline void setInput(float* v){
        m_input = v;
    }
    inline float* getOutput(){
        return m_output;
    }
    
    /**
     * @return: 表示是否全部计算完成，会检查每个子线程的计算
     */
    bool newFrame(int matrix = -1);
    /**
     * @A:  系数矩阵
     * @B:  输入向量
     * @O:  输出
     * @w:  矩阵宽
     * @h:  矩阵高
     */
    void matmul(float* A, float* B, float* b, float* O, int w, int h);
    
    void setAsyncThreads(ThreadPool* pool);
    
    float*      m_weights;
    float*      m_bias;
    int         m_width;
    int         m_height;
    
    float*      m_output;
    float*      m_input;
    
    GPUPixelBuffer* m_pixel_buffer; // 用于计算完成后释放pbo
    
    // 用于矩阵分解计算
    ThreadPool*             m_threadpool;   // 异步线程池
    pthread_mutex_t         m_async_lock;
    vector<bool>            m_async_flag;   // 标记对应线程是否已经计算完成
};

#endif

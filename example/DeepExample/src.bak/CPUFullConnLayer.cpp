/**
 * file :	CPUFullConnLayer.cpp
 * author :	Rex
 * create :	2018-01-01 13:56
 * func : 
 * history:
 */

#include <arm_neon.h>
#include "bs.h"
#include "CPUFullConnLayer.h"

CPUFullConnLayer::CPUFullConnLayer(float* wei, float* bias, int w, int h):
GPULayerBase(1){
    m_weights = NULL;
    m_bias = NULL;
    m_output = NULL;
    m_threadpool = NULL;
    m_pixel_buffer = NULL;
    setWeights(wei, bias, w, h);
}

CPUFullConnLayer::~CPUFullConnLayer(){
    if (m_weights!=NULL) {
        free(m_weights);
    }
    if (m_output!=NULL) {
        free(m_output);
    }
    if (m_bias!=NULL) {
        free(m_bias);
    }
}

bool CPUFullConnLayer::newFrame(int matrix){
    if (m_threadpool==NULL || matrix < 0) {
        matmul(m_weights, m_input, m_bias, m_output, m_width, m_height);
        return true;
    }
    else if(matrix < m_threadpool->thread_num()){
        int bias = (m_height/m_threadpool->thread_num())*matrix;
        matmul(m_weights + bias*m_width, m_input, m_bias+bias, m_output+bias, m_width, m_height/m_threadpool->thread_num());
        
        // 设置计算标记位并检查
        pthread_mutex_lock(&m_async_lock);
        m_async_flag[matrix] = true;
        for (int i=0; i<m_threadpool->thread_num(); i++) {
            if (!m_async_flag[i]) {
                pthread_mutex_unlock(&m_async_lock);
                return false;
            }
        }
        for (int i=0; i<m_threadpool->thread_num(); i++) {
            m_async_flag[i] = false;
        }
        pthread_mutex_unlock(&m_async_lock);
        diff();
        return true;
    }
    
    return false;
}

void CPUFullConnLayer::setAsyncThreads(ThreadPool* pool){
    m_threadpool = pool;
    m_async_flag.resize(m_threadpool->thread_num());
    pthread_mutex_init(&m_async_lock, NULL);
}

void CPUFullConnLayer::setWeights(float* weight, float* bias, int w, int h){
    m_weights = (float*)malloc(w*h*sizeof(float));
    assert(m_weights!=NULL);
    m_bias = (float*)malloc(h*sizeof(float));
    assert(m_bias!=NULL);
    memcpy(m_weights, weight, w*h*sizeof(float));
    memcpy(m_bias, bias, h*sizeof(float));
    /*
    for (int i=0; i<h; i++) {
        memcpy(m_weights+(w+1)*i, weight+w*i, w*sizeof(float));
        *(m_weights+(w+1)*i+w) = bias[i];
    }
     */
    m_width = w;
    m_height = h;
    m_output = (float*)malloc(h*sizeof(float));
    assert(m_output!=NULL);
}

void CPUFullConnLayer::matmul(float* A, float* B, float* b, float* O, int w, int h){
    float32x4_t cmp = vdupq_n_f32(0.0);
    for (int hi=0; hi<h>>2; hi++) {
        float32x4_t v0 = vdupq_n_f32(0.0);
        float32x4_t v1 = vdupq_n_f32(0.0);
        float32x4_t v2 = vdupq_n_f32(0.0);
        float32x4_t v3 = vdupq_n_f32(0.0);
        int h0 = hi<<2;
        int h1 = h0+1;
        int h2 = h0+2;
        int h3 = h0+3;
        for (int wi=0; wi<w>>2; wi++) {
            int wb = wi<<2;
            float32x4_t vb0 = vld1q_f32(B+wb);
            // 区别与vmlaq_f32, vfmaq_f32在叠加前乘法结果没有做round
            v0 = vmlaq_f32(v0, vld1q_f32(A+h0*w+wb), vb0);
            v1 = vmlaq_f32(v1, vld1q_f32(A+h1*w+wb), vb0);
            v2 = vmlaq_f32(v2, vld1q_f32(A+h2*w+wb), vb0);
            v3 = vmlaq_f32(v3, vld1q_f32(A+h3*w+wb), vb0);
        }
        // vgetq_lane_f32(v0, 0)
        
        O[h0] = vaddvq_f32(v0) + b[h0];
        O[h1] = vaddvq_f32(v1) + b[h1];
        O[h2] = vaddvq_f32(v2) + b[h2];
        O[h3] = vaddvq_f32(v3) + b[h3];
//        if (O[h0]<0 || O[h1]<0 || O[h2]<0 || O[h3]<0) {
//            printf("%d: %f %f %f %f\n", h0, O[h0], O[h1], O[h2], O[h3]);
//        }
        // relu
        float32x4_t o0 = vld1q_f32(O+h0);
        o0 = vmaxq_f32(o0, cmp);
        vst1q_f32(O+h0, o0);
    }
}

void CPUFullConnLayer::diff(){
    if (m_values==NULL) {
        return;
    }
    for (int i=0; i<m_height; i++) {
        if (abs(m_values[i]-m_output[i])>5) {
            printf("%d: %f %f\n", i, m_output[i], m_values[i]);
        }
    }
}

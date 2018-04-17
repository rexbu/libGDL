/**
 * file :	CPUFullConnLayer.cpp
 * author :	Rex
 * create :	2018-01-01 13:56
 * func : 
 * history:
 */

#include <arm_neon.h>
#include <iostream>
#include <vector>
#include "bs.h"
#include "CPUFullConnLayer.h"

using namespace std;
// a32的float32x4_t 4参数求和
#define n32_vaddvq_f32(v, o)   do{ \
    float32x2_t _ss = vadd_f32(vget_low_f32(v), vget_high_f32(v));  \
    float32x2_t _sss2 = vpadd_f32(_ss, _ss);                        \
    *(o) = vget_lane_f32(_sss2, 0);                                 \
}while(0)

CPUFullConnLayer::CPUFullConnLayer(conv_active_t active):
m_active(active),
GPULayerBase(1){
    m_weights = NULL;
    m_bias = NULL;
    m_output = NULL;
    m_threadpool = NULL;
    m_pixel_buffer = NULL;
}

CPUFullConnLayer::CPUFullConnLayer(float* wei, float* bias, int w, int h, conv_active_t active):
m_active(active),
GPULayerBase(1, "CPUFullConn"){
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
    // 一般用于最后一层全连接，有可能不是2的倍数
    memset(m_output, 0, sizeof(float)*m_height);
    if (m_threadpool==NULL || matrix < 0) {
        matmul(m_weights, m_input, m_bias, m_output, m_width, m_height);
        if (m_height%4!=0) {
            int has_cal = (m_height>>2)<<2;
            matmul_line(m_weights+has_cal*m_width, m_input, m_bias+has_cal, m_output+has_cal, m_width, m_height-has_cal);
        }
        // diff();
    }
    // 矩阵行也就是输出必须是2的倍数
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
        
        // diff();
    }
    
    return true;
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

void CPUFullConnLayer::reshape(int c_width, int c_height, int c_x_count, int c_y_count, int c_count){
    vector<float> wei;
    if (c_count==0) {
        c_count = c_x_count * c_y_count;
    }
    
    // 遍历卷积核
    int kernel_count = m_height;
    int kernel_line = c_width*c_height*c_count*4;
    for(int k=0; k<kernel_count; k++){
        // 遍历通道宽高
        for (int h=0;  h<c_height*c_y_count; h++){
            for (int w=0; w<c_width*c_x_count; w++){
                // 属于第几个通道
                int c_h = h/c_height;
                int c_w = w/c_width;
                int c = (c_h*c_x_count + c_w)*4;
                // 在通道内的坐标
                int c_y = h%c_height;
                int c_x = w%c_width;
                for (int ci = c; ci<c+4; ci++) {
                    // tensorflow的reshape函数将所有的通道放在一起
                    int i = c_width*c_height*ci + c_y*c_width + c_x;
                    wei.push_back(m_weights[k*kernel_line + i]);
                }
            }
        }
    }
    
    memcpy(m_weights, &wei[0], m_width*m_height*sizeof(float));
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
#if __IOS__
        O[h0] = vaddvq_f32(v0) + b[h0];
        O[h1] = vaddvq_f32(v1) + b[h1];
        O[h2] = vaddvq_f32(v2) + b[h2];
        O[h3] = vaddvq_f32(v3) + b[h3];
#else
        // vaddvq_f32只支持A64，安卓一般都使用32位so
        O[h0] = vgetq_lane_f32(v0,0)+vgetq_lane_f32(v0,1)+vgetq_lane_f32(v0,2)+vgetq_lane_f32(v0,3) + b[h0];
        O[h1] = vgetq_lane_f32(v1,0)+vgetq_lane_f32(v1,1)+vgetq_lane_f32(v1,2)+vgetq_lane_f32(v1,3) + b[h1];
        O[h2] = vgetq_lane_f32(v2,0)+vgetq_lane_f32(v2,1)+vgetq_lane_f32(v2,2)+vgetq_lane_f32(v2,3) + b[h2];
        O[h3] = vgetq_lane_f32(v3,0)+vgetq_lane_f32(v3,1)+vgetq_lane_f32(v3,2)+vgetq_lane_f32(v3,3) + b[h3];
        /*
        n32_vaddvq_f32(v0, O+h0);
        O[h0] += b[h0];
        n32_vaddvq_f32(v1, O+h1);
        O[h1] += b[h1];
        n32_vaddvq_f32(v2, O+h2);
        O[h2] += b[h2];
        n32_vaddvq_f32(v3, O+h3);
        O[h3] += b[h3];
        */
#endif
        // relu
        switch (m_active) {
            case CONV_ACTIVE_RELU:{
                // 一次对比4个数，就是h0-h3
                float32x4_t o0 = vmaxq_f32(vld1q_f32(O+h0), cmp);
                vst1q_f32(O+h0, o0);
            }
                break;
                
            default:
                break;
        }
    }
}

void CPUFullConnLayer::matmul_line(float* A, float* B, float* b, float* O, int w, int h){
    float32_t cmp = 0.0;
    for (int hi=0; hi<h; hi++) {
        float32x4_t v0 = vdupq_n_f32(0.0);
        int h0 = hi;
        for (int wi=0; wi<w>>2; wi++) {
            int wb = wi<<2;
            float32x4_t vb0 = vld1q_f32(B+wb);
            // 区别与vmlaq_f32, vfmaq_f32在叠加前乘法结果没有做round
            v0 = vmlaq_f32(v0, vld1q_f32(A+h0*w+wb), vb0);
        }
        // vgetq_lane_f32(v0, 0)
#if __IOS__
        O[h0] = vaddvq_f32(v0) + b[h0];
#else
        // vaddvq_f32只支持A64，安卓一般都使用32位so
        O[h0] = vgetq_lane_f32(v0,0)+vgetq_lane_f32(v0,1)+vgetq_lane_f32(v0,2)+vgetq_lane_f32(v0,3) + b[h0];
        /*
        n32_vaddvq_f32(v0, O+h0);
        O[h0] += b[h0];
        */
#endif
        float32_t o0 = O[h0];
        switch (m_active) {
            case CONV_ACTIVE_RELU:
                O[h0] = o0>cmp ? o0:cmp;
                break;
                
            default:
                break;
        }
    }
}

void CPUFullConnLayer::diff(){
    if (m_values==NULL) {
        return;
    }
    for (int i=0; i<m_height; i++) {
        if (abs(m_values[i]-m_output[i])>1) {
            err_log("%d: %f %f\n", i, m_output[i], m_values[i]);
        }
    }
}

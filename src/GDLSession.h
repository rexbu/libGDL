/**
 * file :	GDLSession.h
 * author :	Rex
 * create :	2017-12-14 22:22
 * func : 
 * history:
 */

#ifndef	__GDLSESSION_H_
#define	__GDLSESSION_H_

#include "GDLGlobal.h"
#include "ThreadPool.h"
#include "GDLModel.h"

// 异步计算全连接层传入的参数
typedef struct __async_fullconn_para{
    CPUFullConnLayer* layer;
    int matrix_id;  // 计算第几个子矩阵
}__async_fullconn_para;

class GDLSession: public GPUGroupFilter{
public:
    GDLSession(bool sync = false);
    GDLSession(const char* path, bool sync = false);
    ~GDLSession();
    
    bool loadModel(const char* path);
    
    // 设置输入归一化
    bool normalize(int normal_size);
    bool addLayerBase(GPULayerBase* base, const char* name=NULL);
    // 添加卷积层
    bool addConv(GPUConvLayer* layer, const char* name);
    
    // 添加池化层
    bool addPool(GPUPoolLayer* pool, const char* name=NULL);
    // 添加全连接层
    bool addFullConv(CPUFullConnLayer* full, const char* name=NULL);
    
    virtual void newFrame();
    
    float* getOutput();
    uint32_t getOutCount();
    
    // 总层数
    inline uint32_t layerCount(){
        return (uint32_t)m_conv_layers.size()+(uint32_t)m_full_layers.size();
    }
    inline GPULayerBase* layer(int i){
        if (i<0 || i >= m_conv_layers.size()+m_full_layers.size()) {
            return NULL;
        }
        
        if(i>=m_conv_layers.size()){
            return m_full_layers[i-m_conv_layers.size()].first;
        }
        return m_conv_layers[i].first;
    }
    
    inline GPULayerBase* layer(const char* name){
        for (int i=0; i<m_conv_layers.size(); i++) {
            if (m_conv_layers[i].second == name) {
                return m_conv_layers[i].first;
            }
        }
        for (int i=0; i<m_full_layers.size(); i++) {
            if (m_full_layers[i].second == name) {
                return m_full_layers[i].first;
            }
        }
        return NULL;
    }
    
protected:
    void async();
    
    GDLModel*               m_model;
    int                     m_normal_size;  // 归一化尺寸
    
    GPUConvShape*           m_conv_shape;
    GPUPixelBuffer*         m_pixel_buffer;
    float*                  m_pixel_float;
    
    vector<pair<GPULayerBase*, string> >     m_conv_layers;
    vector<pair<CPUFullConnLayer*,string> >  m_full_layers;
    
    vector<float>           m_outdata;
    uint8_t*                m_buffer;
    
    bool                    m_fullwei_reshape;  // 第一次计算全连接层时候需要对卷积核进行重排
    ThreadPool*             m_threadpool;
    bool                    m_sync;
    vector<__async_fullconn_para>   m_async_para;
};

#endif

/**
 * file :	GDLSession.cpp
 * author :	Rex
 * create :	2017-12-14 22:22
 * func : 
 * history:
 */
#include <arm_neon.h>
#include "GDLSession.h"

GDLSession::GDLSession(bool sync):
m_sync(sync){
    m_model = NULL;
    m_conv_shape = NULL;
    m_pixel_buffer = NULL;
    m_threadpool = NULL;
    m_fullwei_reshape = false;
}

GDLSession::GDLSession(const char* path, bool sync):
m_sync(sync){
    m_threadpool=NULL;
    m_conv_shape = NULL;
    m_pixel_buffer = NULL;
    m_fullwei_reshape = false;
    loadModel(path);
}

bool GDLSession::loadModel(const char *path){
    m_model = new GDLModel(path);
    if (m_model->loaded()) {
        return true;
    }
    else{
        delete m_model;
        m_model = NULL;
        return false;
    }
}

GDLSession::~GDLSession(){
    if (m_conv_shape!=NULL) {
        delete m_conv_shape;
    }
    
    if (m_threadpool!=NULL) {
        delete m_threadpool;
    }
    
    if (m_model!=NULL) {
        delete m_model;
    }
    
    for (int i=0; i<m_conv_layers.size(); i++) {
        delete m_conv_layers[i].first;
    }
    
    if (m_pixel_buffer!=NULL) {
        delete m_pixel_buffer;
        free(m_pixel_float);
    }
}

bool GDLSession::normalize(int normal_size){
    // 计算reshape的padding尺寸，也就是第一层卷积需要的padding大小
    m_conv_shape = new GPUConvShape(normal_size, 3, 0);
    m_conv_layers.push_back(pair<GPULayerBase*, string>(m_conv_shape, "shape"));
    m_input = m_conv_shape;
    return true;
}

bool GDLSession::addLayerBase(GPULayerBase* base, const char* name){
    if (m_conv_layers.size()>0) {
        m_conv_layers[m_conv_layers.size()-1].first->addTarget(base);
    }
    else{
        m_input = base;
    }
    if (name==NULL) {
        name = "";
    }
    m_conv_layers.push_back(pair<GPULayerBase*, string>(base, name));
    m_output = base;
    return true;
}

bool GDLSession::addConv(GPUConvLayer* layer, const char* name){
    if (m_conv_layers.size()>0) {
        m_conv_layers[m_conv_layers.size()-1].first->addTarget(layer);
    }
    else{
        m_input = layer;
    }
    conv_layer_t* wei = m_model->layer(name);
    if (wei==NULL) {
        err_log("libGDL Error: can't find kernel[%s] in model", name);
        return false;
    }
    
    layer->setConvKernel(wei->weight, wei->kernel_width*wei->kernel_height*wei->kernel_count*wei->channel_count);
    layer->setBias(wei->bias, wei->kernel_count);
    
    m_conv_layers.push_back(pair<GPULayerBase*, string>(layer, name));
    m_output = layer;
    return true;
}

bool GDLSession::addPool(GPUPoolLayer* pool, const char* name){
    if (m_conv_layers.size()>0) {
        m_conv_layers[m_conv_layers.size()-1].first->addTarget(pool);
    }
    else{
        m_input = pool;
    }
    
    if (name==NULL) {
        name = "";
    }
    m_conv_layers.push_back(pair<GPULayerBase*, string>(pool, name));
    m_output = pool;
    return true;
}

bool GDLSession::addFullConv(CPUFullConnLayer* full, const char* name){
    if (m_full_layers.size()>0) {
        m_full_layers[m_full_layers.size()-1].first->addTarget(full);
    }
    else{
        /*
        // buffer长度为全连接层输入向量维度
        m_pixel_buffer = new GPUPixelBuffer(full->m_width, m_sync);
//        m_pixel_float = (float*)malloc(full->m_width*sizeof(float));
//        assert(m_pixel_buffer!=NULL && m_pixel_float!=NULL);
        // 在异步过程后需要释放pbo
        full->m_pixel_buffer = m_pixel_buffer;
         */
    }
    
    if (name==NULL) {
        name = "";
    }
    conv_layer_t* wei = m_model->layer(name);
    if (wei==NULL) {
        err_log("libGDL Error: can't find kernel[%s] in model", name);
        return false;
    }
    
    full->setWeights(wei->weight, wei->bias, wei->kernel_width, wei->kernel_count);
    m_full_layers.push_back(pair<CPUFullConnLayer*, string>(full, name));
    m_output = full;
    return true;
}

static __async_fullconn_para g_default_para;
void* run_fullconn(void* p){
    __async_fullconn_para* para = (__async_fullconn_para*)p;
    CPUFullConnLayer* layer = para->layer;
    struct timeval t0,t1;
    gettimeofday(&t0, NULL);
    bool st = layer->newFrame(para->matrix_id);
    gettimeofday(&t1, NULL);
    //err_log("full frame:%d time:%lu %lu\n", para->matrix_id, (t1.tv_sec-t0.tv_sec)*1000+(t1.tv_usec-t0.tv_usec)/1000, t0.tv_sec*1000+t0.tv_usec/1000);
    // 计算完成，解除pbo
    if (st) {
        if (layer->m_pixel_buffer!=NULL) {
            // layer->m_pixel_buffer->unlock();
        }
        // 如果还有下个全连接层，则计算
        if (layer->targetsCount()>0) {
            int location;
            CPUFullConnLayer* next = (CPUFullConnLayer*)layer->getTarget(0, location);
            next->setInput(layer->getOutput());
            g_default_para.layer = next;
            g_default_para.matrix_id = -1;
            layer->m_threadpool->add(run_fullconn, &g_default_para);
        }
    }
    
    return NULL;
}

void GDLSession::newFrame(){
    async();
    
    struct timeval t0,t1;
    gettimeofday(&t0, NULL);
    for (int i=0; i<m_conv_layers.size(); i++) {
        m_conv_layers[i].first->newFrame();
    }
    gettimeofday(&t1, NULL);
    //err_log("**** conv layer: time:%lu %lu ****\n", (t1.tv_sec-t0.tv_sec)*1000+(t1.tv_usec-t0.tv_usec)/1000, t0.tv_sec*1000+t0.tv_usec/1000);
    
    m_outbuffer = m_conv_layers[m_conv_layers.size()-1].first->m_outbuffer;
    ConvFrameBuffer* buffer = (ConvFrameBuffer*)m_outbuffer;
    /*
    if (m_pixel_buffer==NULL) {
        m_pixel_buffer = new GPUPixelBuffer(m_outbuffer->m_width*m_outbuffer->m_height, m_sync, 4);
    }
    m_pixel_float = (float*)m_pixel_buffer->read(m_outbuffer);
    printf("*********************************");
    if (m_pixel_float==NULL) {
        return;
    }
    for (int h=0; h<32; h++) {
        for (int w=0; w<32; w++) {
            printf("%0.0f ", m_pixel_float[h*32*4+w*4]);
        }
        printf("\n");
    }
    printf("\n");
    */
    // 开始读
    if (m_full_layers.size()>0) {
        gettimeofday(&t0, NULL);
        // uint8_t* rgba = m_outbuffer->getPixels();
        if (m_pixel_buffer==NULL) {
            m_pixel_buffer = new GPUPixelBuffer(m_outbuffer->m_width*m_outbuffer->m_height, m_sync, 4);
            m_full_layers[0].first->m_pixel_buffer = m_pixel_buffer;
        }
        m_pixel_float = (float*)m_pixel_buffer->read(m_outbuffer);
        gettimeofday(&t1, NULL);
        //err_log("readPixel: time:%lu %lu\n", (t1.tv_sec-t0.tv_sec)*1000+(t1.tv_usec-t0.tv_usec)/1000, t0.tv_sec*1000+t0.tv_usec/1000);
        if (m_pixel_float==NULL) {
            return;
        }
        
        if (!m_fullwei_reshape) {
            // m_full_layers[0].first->reshape(buffer->m_channel_width, buffer->m_channel_height, buffer->m_x_count, buffer->m_y_count);
            m_fullwei_reshape = true;
        }
        
        if (m_sync) {
            float* out = m_pixel_float;
            for (int i=0; i<m_full_layers.size(); i++) {
                m_full_layers[i].first->setInput(out);
                gettimeofday(&t0, NULL);
                m_full_layers[i].first->newFrame();
                gettimeofday(&t1, NULL);
                printf("frame: time:%lu\n", (t1.tv_sec-t0.tv_sec)*1000+(t1.tv_usec-t0.tv_usec)/1000);
                out = m_full_layers[i].first->getOutput();
            }
        }
        else{
            m_full_layers[0].first->setInput(m_pixel_float);
            for (int i=0; i<m_threadpool->thread_num(); i++) {
                m_async_para[i].layer = m_full_layers[0].first;
                m_async_para[i].matrix_id = i;
                m_threadpool->add(run_fullconn, &m_async_para[i]);
            }
        }
    }
    
    informTargets();
}

void GDLSession::async(){
    // 全连接层有几个建立几个异步线程
    if (!m_sync && m_threadpool==NULL && m_full_layers.size()>0) {
        m_threadpool = new ThreadPool((uint32_t)m_full_layers.size());
        m_async_para.resize(m_full_layers.size());
        m_full_layers[0].first->setAsyncThreads(m_threadpool);
        for (int i=0; i<m_full_layers.size()-1; i++) {
            m_full_layers[i].first->m_threadpool = m_threadpool;
        }
    }
}

float* GDLSession::getOutput(){
    m_outdata.clear();
    uint8_t* rgba = (uint8_t*)malloc(m_outbuffer->m_width*m_outbuffer->m_height*4);
    m_outbuffer->getPixels(rgba);
    for (int h=0; h<m_outbuffer->m_height; h++) {
        for (int w=0; w<m_outbuffer->m_width; w++) {
            uint32_t r = rgba[h*m_outbuffer->m_width*4+w*4];
            uint32_t g = rgba[h*m_outbuffer->m_width*4+w*4+1];
            uint32_t b = rgba[h*m_outbuffer->m_width*4+w*4+2];
            uint32_t a = rgba[h*m_outbuffer->m_width*4+w*4+3];
            uint32_t ii = r*256*256*256 + g*256*256 + b*256 + a;
            float f;
            memcpy(&f, &ii, sizeof(float));
            m_outdata.push_back(f);
        }
    }
    free(rgba);
    return &m_outdata[0];
}

uint32_t GDLSession::getOutCount(){
    return dynamic_cast<GPULayerBase*>(m_output)->m_kernel_count;
}

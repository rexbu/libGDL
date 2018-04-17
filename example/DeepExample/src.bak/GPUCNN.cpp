/**
 * file :	GPUCNN.cpp
 * author :	Rex
 * create :	2017-12-02 22:56
 * func : 
 * history:
 */

#include "GPUCNN.h"
#include "ConvFrameBuffer.h"

#pragma --mark "卷积层基类"

GPULayerBase::GPULayerBase(int kernel_count, const char* name):
m_kernel_count(kernel_count),
GPUFilter(false, name){
    m_last_layer = NULL;
    m_values = NULL;
    m_out = NULL;
    // 计算输出通道排列
    if (!bs_is_powerof2(kernel_count)) {
        // 一般只有第一层rgb展开卷积核数量不是2的次幂
        err_log("kernel count[%d] is not power of 2!", kernel_count);
        m_outx_count = ceil(kernel_count*1.0/4);
        m_outy_count = 1;
    }
    else{
        float power = log2(m_kernel_count/4);
        m_outx_count = pow(2, ceil(power/2));
        m_outy_count = pow(2, floor(power/2));
    }
}

void GPULayerBase::newFrame(){
    // setInputFrameBuffer(m_last_layer->m_outbuffer, 0);
    render();
    for(int i = 0; i < m_targets.size(); i ++) {
        GPUInput* input = m_targets[i];
        input->setInputFrameBuffer(m_outbuffer, m_location[i]);
    }
    // print(m_outbuffer, false, m_filter_name.c_str());
    // diff();
    // debug();
}
/*
void GPULayerBase::activeOutFrameBuffer(){
    if (m_outbuffer==NULL) {
        gpu_size_t size = sizeOfFBO();
        m_outbuffer = new GPUFrameBuffer(size.width, size.height, GPUFrameBuffer::uintFrameOption(), false);
        
        m_coordinate_buffer = new GPUVertexBuffer();
        m_vertex_buffer = new GPUVertexBuffer();
        m_vertex_buffer->setBuffer(&m_vertices[0]);
        m_coordinate_buffer->setBuffer(GPUFilter::coordinatesRotation(GPUNoRotation));
        
    }
    
    m_outbuffer->activeBuffer();
    glClearColor(m_clear_color[0], m_clear_color[1], m_clear_color[2], m_clear_color[3]);
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}
*/
GPUFrameBuffer* GPULayerBase::floatFrameBuffer(float* v, int w, int h){
    gpu_frame_option_t option;
    memcpy(&option, GPUFrameBuffer::nearestFrameOption(), sizeof(gpu_frame_option_t));
    option.color_format = GL_RGBA32F;
    option.format = GL_RGBA;
    option.type = GL_FLOAT;
    
    GPUFrameBuffer* buffer = new GPUFrameBuffer(w, h, &option, true);
    buffer->setPixels(v);
    return buffer;
}

void GPULayerBase::setInputFrameBuffer(GPUFrameBuffer* buffer, int location)
{
    if(location >= m_inputs || location<0) {
        err_log("GPUInput::setInputFrameBuffer(%d) overflow!", location);
        return;
    }
    if (buffer==NULL) {
        err_log("setInputFrameBuffer NULL!");
        return;
    }
    
    m_input_buffers[location] = buffer;
    
    if(location == 0){
        m_firstbuffer = buffer;
        // 存储帧大小
        if (m_frame_width!=buffer->m_width || m_frame_height!=buffer->m_height)
        {
            setFrameSize(buffer->m_width, buffer->m_height);
        }
    }
}

void GPULayerBase::setFrameSize(uint32_t width, uint32_t height){
    GPUFilter::setFrameSize(width, height);
}

void GPULayerBase::print(GPUFrameBuffer* buffer, bool isrgba, const char* name){
    if (name!=NULL) {
        printf("##### %s:%d/%d ####\n", name, buffer->m_width, buffer->m_height);
    }
    else{
        printf("##### %d/%d ####\n", buffer->m_width, buffer->m_height);
    }
    uint8_t* rgba = (uint8_t*)malloc(buffer->m_width*buffer->m_height*4);
    buffer->getPixels(rgba);
    for (int h=0; h<buffer->m_height; h++) {
        for (int w=0; w<buffer->m_width; w++) {
            uint32_t r = rgba[h*buffer->m_width*4+w*4];
            uint32_t g = rgba[h*buffer->m_width*4+w*4+1];
            uint32_t b = rgba[h*buffer->m_width*4+w*4+2];
            uint32_t a = rgba[h*buffer->m_width*4+w*4+3];
            if (isrgba) {
                printf("%d/%d/%d/%d\t\t", r, g, b, a);
            }
            else{
                uint32_t ii = r*256*256*256 + g*256*256 + b*256 + a;
                float f;
                memcpy(&f, &ii, sizeof(float));
                printf("%0.4f\t", f);
            }
        }
        printf("\n");
    }
    
    free(rgba);
}

void GPULayerBase::setTensor(const char* name){
    struct stat statbuff;
    stat(name, &statbuff);
    m_values = (float*)malloc(statbuff.st_size);
    FILE* fp = fopen(name, "r");
    fread(m_values, statbuff.st_size, 1, fp);
    fclose(fp);
}

void GPULayerBase::diff(){
    if (m_values == NULL) {
        return;
    }
    
    float* rgba = (float*)malloc(m_outbuffer->m_width*m_outbuffer->m_height*4*4);
    m_outbuffer->getPixels((uint8_t*)rgba);
    ConvFrameBuffer* buffer = dynamic_cast<ConvFrameBuffer*>(m_outbuffer);
    for (int hc=0; hc<buffer->m_y_count; hc++) {
        for (int wc=0; wc<buffer->m_x_count; wc++) {
            int x_start = wc*(buffer->m_channel_width);
            int y_start = hc*(buffer->m_channel_height);
            
            for (int h=0; h<buffer->m_channel_height; h++) {
                for (int w=0; w<buffer->m_channel_width; w++) {
                    float r = rgba[(h+y_start)*m_outbuffer->m_width*4+(w+x_start)*4];
                    float g = rgba[(h+y_start)*m_outbuffer->m_width*4+(w+x_start)*4+1];
                    float b = rgba[(h+y_start)*m_outbuffer->m_width*4+(w+x_start)*4+2];
                    float a = rgba[(h+y_start)*m_outbuffer->m_width*4+(w+x_start)*4+3];
                    
                    float f1 = m_values[(hc*buffer->m_x_count + wc)*(buffer->m_channel_width*buffer->m_channel_height)*4 + h*buffer->m_channel_width+w];
                    float f2 = m_values[(hc*buffer->m_x_count + wc)*(buffer->m_channel_width*buffer->m_channel_height)*4 + (buffer->m_channel_width*buffer->m_channel_height) + h*buffer->m_channel_width+w];
                    float f3 = m_values[(hc*buffer->m_x_count + wc)*(buffer->m_channel_width*buffer->m_channel_height)*4 + (buffer->m_channel_width*buffer->m_channel_height)*2 + h*buffer->m_channel_width+w];
                    float f4 = m_values[(hc*buffer->m_x_count + wc)*(buffer->m_channel_width*buffer->m_channel_height)*4 + (buffer->m_channel_width*buffer->m_channel_height)*3 + h*buffer->m_channel_width+w];
                    if (abs(r-f1)>0.01) {
                        printf("diff pos[r:%d/%d/%d] val[%f, %f]\n", (hc*buffer->m_x_count+wc)*4, w,h, r, f1);
                    }
                    if (abs(g-f2)>0.01) {
                        printf("diff pos[g:%d/%d/%d] val[%f, %f]\n", (hc*buffer->m_x_count+wc)*4+1, w,h, g, f2);
                    }
                    if (abs(b-f3)>0.01) {
                        printf("diff pos[b:%d/%d/%d] val[%f, %f]\n", (hc*buffer->m_x_count+wc)*4+2, w,h, b, f3);
                    }
                    if (abs(a-f4)>0.01) {
                        printf("diff pos[a:%d/%d/%d] val[%f, %f]\n", (hc*buffer->m_x_count+wc)*4+3, w,h, a, f4);
                    }
                }
            }
        }
    }
    
    free(rgba);
}

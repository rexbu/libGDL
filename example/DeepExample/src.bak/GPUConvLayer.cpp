/**
 * file :	GPUConvLayer.cpp
 * author :	Rex
 * create :	2017-11-25 18:32
 * func : 
 * history:
 */
#include "ConvFrameBuffer.h"
#include "GPUConvLayer.h"

#pragma --mark "通道卷积GPUConvChannel"

static const char* g_conv_fragment = SHADER30_STRING(
    in highp vec2 textureCoordinate;
    out highp uvec4 out_color;

    // 加上了padding尺寸, 0、1是输入纹理channel宽高
    uniform int channel_size[2];
    // 0位x方向通道数，1位y方向通道数，0、1为输入，2、3为输出
    uniform int channel_count[2];
    // 纹理步长
    uniform highp float texture_step[2];
    // 卷积核纹理步长
    uniform highp float kernel_step[2];
    // 0为输入纹理，1为卷积核, 2为偏移
    uniform highp sampler2D inputImageTexture[3];
    // 卷积核大小
    const int kernel_size = %d;
    uniform highp float bias[%d];
                                                           
#define encodeFloats(v) uvec4(floatBitsToUint((v).r), floatBitsToUint((v).g), floatBitsToUint((v).b), floatBitsToUint((v).a))
#define decodeFloats(v) vec4(uintBitsToFloat((v).r), uintBitsToFloat((v).g), uintBitsToFloat((v).b), uintBitsToFloat((v).a))
                      
     highp float conv(highp vec4 bias_v){
         highp float total = 0.0;
         highp vec4 color;
         highp vec4 conv;
         for (int h=0; h<channel_count[1]; h++){
             for(int w=0; w<channel_count[0]; w++){
                 // 第(w,h)通道处卷积计算起始坐标
                 highp float x = float(w*channel_size[0])*texture_step[0] + bias_v.r;
                 highp float y = float(h*channel_size[1])*texture_step[1] + bias_v.g;
                 // 第几个通道
                 int ci = h*channel_count[0] + w;
                 // 卷积核的起始x坐标
                 float w_x = float(ci*kernel_size)*kernel_step[0] + kernel_step[0]/2.0;
                 for (int i = 0; i < kernel_size*kernel_size; ++i){
                     int y_i = i / kernel_size;
                     int x_i = i - y_i*kernel_size;
                     // 通道像素点
                     color = texture(inputImageTexture[0], vec2(x,y) + vec2(texture_step[0]*float(x_i), texture_step[1]*float(y_i)));
                     // 卷积核
                     conv = texture(inputImageTexture[1], vec2(w_x, bias_v.b) + vec2(kernel_step[0]*float(x_i), kernel_step[1]*float(y_i)));
                     total += dot(color, conv);
                 }
             }
         }
         
         total += bias[uint(bias_v.a)];
         return %s;
     }
    void main(){
        // r为x偏移，g为y偏移，b为卷积核y偏移,a为第几个卷积核
        highp vec4 bias_v = texture(inputImageTexture[2], textureCoordinate);
        
        highp vec4 conv_result;
        conv_result.r = conv(bias_v);
        bias_v.b += float(kernel_size)*kernel_step[1];
        bias_v.a += 1.0;
        conv_result.g = conv(bias_v);
        
        bias_v.b += float(kernel_size)*kernel_step[1];
        bias_v.a += 1.0;
        conv_result.b = conv(bias_v);
        
        bias_v.b += float(kernel_size)*kernel_step[1];
        bias_v.a += 1.0;
        conv_result.a = conv(bias_v);
        
        out_color = encodeFloats(conv_result);
        // out_color = encodeFloat(bias[uint(bias_v.a)]);
     }
);

#pragma --mark "卷积层GPUConvLayer"
GPUConvLayer::GPUConvLayer(int kernel_size, int kernel_count, conv_active_t active_type, const char* name):
GPULayerBase(kernel_count, name),
m_kernel_size(kernel_size),
m_active_type(active_type){
    m_inputs = 3;
    m_kernel_buffer = NULL;
    m_map_buffer = NULL;
    m_map_values = NULL;
    m_padding_size = m_kernel_size/2;
    
    char shader[10240];
    switch (active_type) {
        case CONV_ACTIVE_NULL:
            snprintf(shader, sizeof(shader), g_conv_fragment, kernel_size, kernel_count, "total");
            break;
        case CONV_ACTIVE_RELU:
            snprintf(shader, sizeof(shader), g_conv_fragment, kernel_size, kernel_count, "max(total, 0.0)");
            break;
        default:
            break;
    }
    
    m_program = new GPUProgram(g_vertext30_shader, shader, m_filter_name.c_str());
    init();
    if (!bs_is_powerof2(kernel_count)) {
        err_log("Kernel count[%d] isn't power of 2", kernel_count);
    }
}

GPUConvLayer::~GPUConvLayer(){
    if (m_kernel_buffer!=NULL) {
        delete m_kernel_buffer;
    }
    
    if (m_map_buffer!=NULL) {
        delete m_map_buffer;
    }
    if (m_map_values!=NULL) {
        free(m_map_values);
    }
    GPULayerBase::~GPULayerBase();
}
void GPUConvLayer::render(){
#if DEBUG_FILTER_NAME
    err_log("filter name: %s texture: %d", m_filter_name.c_str(), m_input_buffers[0]->m_texture);
#endif
    
    GPUCheckGlError(m_filter_name.c_str(), true, false);
    GPUContext* context = GPUContext::shareInstance();
    context->glContextLock();   // 加锁，防止此时设置参数
    
    context->setActiveProgram(m_program);
    activeOutFrameBuffer();
    for (int i=0; i<m_inputs; i++) {
        m_input_buffers[i]->activeTexture(GL_TEXTURE0+i);
        glUniform1i(m_input_textures[i], 0+i);
    }
    
    m_coordinate_buffer->activeBuffer(m_input_coordinate);
    m_vertex_buffer->activeBuffer(m_position);
    glDrawElements(GL_TRIANGLES, m_outx_count*m_outy_count*6, GL_UNSIGNED_SHORT, ((ConvFrameBuffer*)m_outbuffer)->vertexIndexs());
    glFlush();
    m_coordinate_buffer->disableBuffer(m_input_coordinate);
    m_vertex_buffer->disableBuffer(m_position);
    m_outbuffer->unactive();
    
    context->glContextUnlock();
}

void GPUConvLayer::activeOutFrameBuffer(){
    if (m_outbuffer == NULL) {
        ConvFrameBuffer* buffer = dynamic_cast<ConvFrameBuffer*>(m_input_buffers[0]);
        m_outbuffer = new ConvFrameBuffer(buffer->m_channel_width, buffer->m_channel_height, 0, m_outx_count, m_outy_count, m_kernel_count/4);
        // 坐标纹理
        // m_map_buffer = mapTexture(buffer->m_channel_width+2*buffer->m_padding_size, buffer->m_channel_height+2*buffer->m_padding_size);
        // 卷积层计算不加padding
        m_map_buffer = mapTexture(buffer->m_channel_width, buffer->m_channel_height);
        m_input_buffers[2] = m_map_buffer;
        
        memset(m_clear_color, 0, sizeof(float)*4);
        m_coordinate_buffer = new GPUVertexBuffer(m_outx_count*m_outy_count*4);
        m_coordinate_buffer->setBuffer(((ConvFrameBuffer*)m_outbuffer)->convCoordinates());
        m_vertex_buffer = new GPUVertexBuffer(m_outx_count*m_outy_count*4);
        m_vertex_buffer->setBuffer(((ConvFrameBuffer*)m_outbuffer)->convVertices());
    }
    m_outbuffer->activeBuffer();
    
    glClearColor(m_clear_color[0], m_clear_color[1], m_clear_color[2], m_clear_color[3]);
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

void GPUConvLayer::setFrameSize(uint32_t width, uint32_t height){
    GPUFilter::setFrameSize(width, height);
    
    ConvFrameBuffer* buffer = dynamic_cast<ConvFrameBuffer*>(m_input_buffers[0]);
    // 参数
    int channel_size[] = {buffer->m_channel_width+2*buffer->m_padding_size, buffer->m_channel_height+2*buffer->m_padding_size};
    m_program->setIntegerv("channel_size", channel_size, 2);
    int channel_count[] = {buffer->m_x_count, buffer->m_y_count};
    m_program->setIntegerv("channel_count", channel_count, 2);
    float texture_step[] = {(float)1.0/buffer->m_width, (float)1.0/buffer->m_height};
    m_program->setFloatv("texture_step", texture_step, 2);
    float kernel_step[] = {(float)1.0/m_kernel_buffer->m_width, (float)1.0/m_kernel_buffer->m_height};
    m_program->setFloatv("kernel_step", kernel_step, 2);
}

void GPUConvLayer::setConvKernel(float* conv_kernel, int size){
    if (size!=m_kernel_size*m_kernel_size*m_channel_count*m_kernel_count) {
        err_log("conv_kernel count[%d] isn't equal to need[%d]", size, m_kernel_size*m_kernel_size*m_channel_count*m_kernel_count);
    }
    if (m_kernel_buffer!=NULL) {
        delete m_kernel_buffer;
    }
    
    // 第一行的r为第一个卷积核，g为第二个卷积核，b为第三个卷积核，a为第四个卷积核
    float* kernel = (float*)calloc(1, sizeof(float)*m_kernel_size*m_kernel_size*ceil(m_channel_count/4.0)*4*m_kernel_count);
    int stride = m_kernel_size*ceil(m_channel_count/4.0)*4;
    for (int h=0; h<m_kernel_size*m_kernel_count; h++) {
        //printf("\nline:%d\n", h);
        int k = h/m_kernel_size;    // 第几个卷积核
        int y = h%m_kernel_size;    // 卷积核内的y坐标
        // 如果通道数不是4的倍数，比如第一层卷积通道数通常为3，那么不足4的通道以0补全
        for (int w=0; w<m_kernel_size*ceil(m_channel_count/4.0); w++) {
            int c = (w/m_kernel_size)*4;    // 通道
            int x = w%m_kernel_size;    // 卷积核内的x坐标
            
            int c_end = c+4;
            if (c_end > m_channel_count) {
                c_end = m_channel_count;
            }
            int ko = h*stride+w*4;
            int co = h*m_kernel_size*m_channel_count + c*m_kernel_size + x*(c_end-c);
            memcpy(kernel+ko, conv_kernel+co, (c_end-c)*sizeof(float));
            //printf("[%d,%d] ", ko, co);
        }
        //printf("\n");
    }
//    float a[] = {
//        -54.939,-70.779,-81.680, -56.939,-72.779,-83.680, -58.939,-74.779,-85.680,
//        -32.939,-48.779,-59.680, -32.939,-48.779,-59.680, -33.939,-49.779,-60.680,
//        -54.939,-70.779,-81.680, -56.939,-72.779,-83.680, -58.939,-74.779,-85.680  };
//    float total = 0;
//    for (int i=0; i<3; i++) {
//        for (int j=0; j<3; j++) {
//            printf("%0.5f,%0.5f,%0.5f ", conv_kernel[i*9+j*3], conv_kernel[i*9+j*3+1], conv_kernel[i*9+j*3+2]);
//            total += conv_kernel[i*9+j*3]*a[i*9+j*3];
//            total += conv_kernel[i*9+j*3+1]*a[i*9+j*3+1];
//            total += conv_kernel[i*9+j*3+2]*a[i*9+j*3+2];
//            //total += kernel[i*12+j*4+3]*a[i*12+j*4+3];
//        }
//        printf("\n");
//    }
//    printf("total:%f\n", total);
    // 宽度为通道数，高度为卷积核数
    m_kernel_buffer = floatFrameBuffer(kernel, m_kernel_size*ceil(m_channel_count/4.0), m_kernel_size*m_kernel_count);
    free(kernel);
    m_input_buffers[1] = m_kernel_buffer;
}

void GPUConvLayer::setBias(float* bias, int size){
    if (size!=m_kernel_count) {
        err_log("bias count[%d] isn't equal to kernel_count[%d]", size, m_kernel_count);
    }
    setFloatv("bias", bias, size);
}

// 此处宽高为channel宽高+padding尺寸，需要加2个padding
GPUFrameBuffer* GPUConvLayer::mapTexture(int channel_width, int channel_height){
    ConvFrameBuffer* outbuffer = dynamic_cast<ConvFrameBuffer*>(m_outbuffer);
    ConvFrameBuffer* inbuffer = dynamic_cast<ConvFrameBuffer*>(m_input_buffers[0]);
    m_map_values = (float*)calloc(1, outbuffer->m_height*outbuffer->m_width*4*sizeof(float));
    // 偏移计算的是在输入纹理中的位置
    float w_step = 1.0/inbuffer->m_width;
    float h_step = 1.0/inbuffer->m_height;
    // printf("#### map texture ####\n");
    for (int h=0; h<outbuffer->m_height; h++) {
        for (int w=0; w<outbuffer->m_width; w++) {
            float x_bias = (w%channel_width)*w_step + w_step/2;
            float y_bias = (h%channel_height)*h_step + h_step/2;
            // 属于第几个输出通道，因为输出中4个通道在一起，所以这里每个坐标对应4个输出通道
            int c_i = ((h/channel_height)*outbuffer->m_x_count + w/channel_width)*4;
            float kernel_y_bias = c_i*m_kernel_size*1.0/m_kernel_buffer->m_height+ 0.5/m_kernel_buffer->m_height;
            // r,g存储在输入纹理channel中偏移，b存储卷积核y偏移，a存储第几个卷积核
            m_map_values[(h*outbuffer->m_width+w)*4] = x_bias;
            m_map_values[(h*outbuffer->m_width+w)*4+1] = y_bias;
            m_map_values[(h*outbuffer->m_width+w)*4+2] = kernel_y_bias;
            m_map_values[(h*outbuffer->m_width+w)*4+3] = c_i;
            // printf("[%f,%f]%f,%d ", x_bias, y_bias, kernel_y_bias, c_i);
            //printf("%0.4f ", x_bias);
            //printf("%0.4f ", y_bias);
            //printf("%0.4f ", kernel_y_bias);
            //printf("%0.4f ", c_i);
        }
        //printf("\n");
    }
    gpu_frame_option_t option;
    memcpy(&option, GPUFrameBuffer::nearestFrameOption(), sizeof(gpu_frame_option_t));
    option.color_format = GL_RGBA32F;
    option.format = GL_RGBA;
    option.type = GL_FLOAT;
    
    GPUFrameBuffer* map_buffer = new GPUFrameBuffer(outbuffer->m_width, outbuffer->m_height, &option, true);
    map_buffer->setPixels(m_map_values);
    return map_buffer;
}

void GPUConvLayer::diff(){
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
                    
                }
            }
        }
    }
    
    free(rgba);
}

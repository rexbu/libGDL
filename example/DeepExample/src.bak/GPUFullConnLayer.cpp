/**
 * file :	GPUFullConnLayer.cpp
 * author :	Rex
 * create :	2017-12-05 11:39
 * func : 
 * history:
 */
#include "ConvFrameBuffer.h"
#include "GPUFullConnLayer.h"

static const char* g_full_conn_fragment = SHADER30_STRING(
     in highp vec2 textureCoordinate;
     out highp vec4 out_color;
    // 0为一维输入向量，1为系数矩阵
     uniform highp sampler2D inputImageTexture[2];
    // 输入纹理的宽高
    uniform highp float w_step;
    uniform highp float h_step;
    uniform highp float dim_step;
    // 系数矩阵的列数，输入向量的维数+1
     uniform highp int dim;
     
     highp vec4 encodeFloat(highp float v ) {
         highp uint uv = floatBitsToUint(v);
         highp uvec4 uvbit = uvec4(uv/uint(256*256*256), uv/uint(256*256), uv/uint(256), uv);
         uvbit -= uvbit.xxyz * uvec4(0, 256, 256, 256);
         return vec4(float(uvbit.x)/255.0, float(uvbit.y)/255.0, float(uvbit.z)/255.0, float(uvbit.w)/255.0);
     }
     
     highp float decodeFloat(highp vec4 v ) {
         highp uint uv = uint(v.x*255.0)*uint(256*256*256) + uint(v.y*255.0)*uint(256*256) + uint(v.z*255.0)*uint(256) + uint(v.w*255.0);
         return uintBitsToFloat(uv);
     }
     
     void main(){
         highp float total = 0.0;
         highp vec4 color;
         highp vec4 weight;
         /*
         int c_count = channel_count[0]*channel_count[1];
         for(int i=0; i<dim-1; i++){
             int h = i/(channel_size[0]*c_count);
             int w = (i - h*(channel_size[0]*c_count))/c_count;
             int c = i - w*c_count - h*(channel_size[0]*c_count);
             int c_yi = c/channel_count[0];
             int c_xi = c - c_yi*channel_count[0];
             
             int in_yi = c_yi*channel_size[1] + h;
             int in_xi = c_xi*channel_size[0] + w;
             color = texture(inputImageTexture[0], vec2((float(in_xi)+0.5)/float(in_size[0]), (float(in_yi)+0.5)/float(in_size[1])));
             highp float v = decodeFloat(color);
             color = texture(inputImageTexture[1], vec2(float(i)*w_step+w_step/2.0, textureCoordinate.y));
             total += (v*color.r);
         }
        */
         int i=0;
         for(highp float h= h_step/2.0; h<1.0; h+=h_step){
             for(highp float w=w_step/2.0; w<1.0; w+=w_step){
                 color = texture(inputImageTexture[0], vec2(w, h));
                 weight = texture(inputImageTexture[1], vec2( (float(i)+0.5)/float(dim), textureCoordinate.y));
                 total += (decodeFloat(color)*weight.r);
                 i++;
             }
         }
         // 加bias
         weight = texture(inputImageTexture[1], vec2( (float(dim)-0.5)/float(dim), textureCoordinate.y));
         total += weight.r;
         %s
     }
 );

GPUFullConnLayer::GPUFullConnLayer(int out_dim, conv_active_t active, const char* name):
m_out_dim(out_dim),
GPULayerBase(out_dim, name){
    m_inputs = 2;
    m_weights = NULL;
    
    char shader[10240];
    // 激励函数，分别为不做处理，relu、sigmod
    const char* active_out[] = {"out_color = encodeFloat(total);", "out_color = encodeFloat(max(total, 0.0));", "out_color = encodeFloat(1.0/(1.0+exp(-1.0*total)));"};
    snprintf(shader, sizeof(shader), g_full_conn_fragment, active_out[active]);
    m_program = new GPUProgram(g_vertext30_shader, shader, m_filter_name.c_str());
    init();
    
    m_weight_buffer  = NULL;
    
    setOutputSize(1, out_dim);
    m_output_vector = (float*)malloc(sizeof(float)*m_out_dim);
    m_option = GPUFrameBuffer::nearestFrameOption();
    memset(m_clear_color, 0, sizeof(float)*4);
}

GPUFullConnLayer::~GPUFullConnLayer(){
    if (m_weight_buffer!=NULL) {
        delete m_weight_buffer;
    }
    if (m_weights!=NULL) {
        free(m_weights);
    }
    free(m_output_vector);
}

void GPUFullConnLayer::setFrameSize(uint32_t width, uint32_t height){
    GPUFilter::setFrameSize(width, height);
    if (m_weight_buffer->m_width != width*height+1) {
        err_log("Weight buffer width[%d] is not equal input vector dimesion[%d]", m_weight_buffer->m_width, width*height+1);
    }
    
    setFloat("w_step", 1.0/width);
    setFloat("h_step", 1.0/height);
    /*
    ConvFrameBuffer* outbuffer =  dynamic_cast<ConvFrameBuffer*>(m_last_layer->m_outbuffer);
    int channel_count[] = {outbuffer->m_x_count, outbuffer->m_y_count};
    m_program->setIntegerv("channel_count", channel_count, 2);
    int channel_size[] = {outbuffer->m_channel_width, outbuffer->m_channel_height};
    m_program->setIntegerv("channel_size", channel_size, 2);
     */
}
void GPUFullConnLayer::setWeights(float* weight, float* bais, int w, int h){
    if (m_weight_buffer!=NULL) {
        delete m_weight_buffer;
    }
    
    // 创建系数矩阵texture
    gpu_frame_option_t option;
    memcpy(&option, GPUFrameBuffer::nearestFrameOption(), sizeof(gpu_frame_option_t));
    option.color_format = GL_R32F;
    option.format = GL_RED;
    option.type = GL_FLOAT;
    // 将bais内置到系数矩阵中，WX + b = [W,b][X,1]
    m_weight_buffer = new GPUFrameBuffer(w+1, h, &option, true);
    GPUCheckGlError("setWeight", true, true);
    m_weights = (float*)malloc((w+1)*h*sizeof(float));
    for (int i=0; i<h; i++) {
        memcpy(m_weights+(w+1)*i, weight+w*i, w*sizeof(float));
        *(m_weights+(w+1)*i+w) = bais[i];
    }
    m_weight_buffer->setPixels(m_weights);
    m_input_buffers[1] = m_weight_buffer;
    GPUCheckGlError("setWeight", true, true);
    // free(new_weights);
    
    setFloat("dim_step", 1.0/(w+1));
    setInteger("dim", w+1);
    GPUCheckGlError("setWeight", true, true);
}

void GPUFullConnLayer::diff(){
    if (m_values == NULL) {
        return;
    }
    /*
    ConvFrameBuffer* buffer = dynamic_cast<ConvFrameBuffer*>(m_last_layer->m_outbuffer);
    int last_size = m_last_layer->m_outbuffer->m_width * m_last_layer->m_outbuffer->m_height;
    
    uint8_t* rgba = (uint8_t*)malloc(last_size * 4);
    m_last_layer->m_outbuffer->getPixels(rgba);
    int count = 0;
    for (int h=0; h<buffer->m_channel_height; h++) {
        for (int w=0; w<buffer->m_channel_width; w++) {
            for (int c=0; c<buffer->m_channel_count; c++) {
                int hc = c/buffer->m_x_count;
                int wc = c - hc*buffer->m_x_count;
                int x_start = wc*(buffer->m_channel_width+buffer->m_padding_size);
                int y_start = hc*(buffer->m_channel_height+buffer->m_padding_size);
                uint32_t r = rgba[(h+y_start)*m_outbuffer->m_width*4+(w+x_start)*4];
                uint32_t g = rgba[(h+y_start)*m_outbuffer->m_width*4+(w+x_start)*4+1];
                uint32_t b = rgba[(h+y_start)*m_outbuffer->m_width*4+(w+x_start)*4+2];
                uint32_t a = rgba[(h+y_start)*m_outbuffer->m_width*4+(w+x_start)*4+3];
                uint32_t ii = r*256*256*256 + g*256*256 + b*256 + a;
                float f1;
                memcpy(&f1, &ii, sizeof(float));
                int i = h*buffer->m_channel_width*buffer->m_channel_count + w*buffer->m_channel_count + c;
                if (c==8 && h==0 && w==0) {
                    printf("%f\n", f1);
                }
                count++;
                m_input_vector[i] = f1;
            }
        }
    }
  
    printf("count: %d\n", count);
    free(rgba);
    m_input_vector[last_size] = 1.0;
    */
    /*
    for (int i=0; i<m_weight_buffer->m_width; i++) {
        if (abs(m_last_layer->m_out[i]-m_values[i])>0.001) {
           printf("%d: %f, %f", i, m_last_layer->m_out[i], m_values[i]);
        }
    }
     */
    if (m_last_layer->m_out == NULL) {
        return;
    }
    
    uint8_t* rgba = (uint8_t*)malloc(m_outbuffer->m_width*m_outbuffer->m_height*4);
    m_outbuffer->getPixels(rgba);
    for (int i=0; i<m_outbuffer->m_width*m_outbuffer->m_height; i++) {
        float total = 0;
        for (int j=0; j<m_last_layer->m_outbuffer->m_width*m_last_layer->m_outbuffer->m_height; j++) {
            total += m_last_layer->m_out[j]*m_weights[i*m_weight_buffer->m_width+j];
            printf("%d: %f-%f,%f\n", j, total, m_last_layer->m_out[j], m_weights[i*m_weight_buffer->m_width+j]);
        }
        total += m_weights[i*m_weight_buffer->m_width+m_weight_buffer->m_width-1];
        
        uint32_t r = rgba[i*4];
        uint32_t g = rgba[i*4+1];
        uint32_t b = rgba[i*4+2];
        uint32_t a = rgba[i*4+3];
        uint32_t ii = r*256*256*256 + g*256*256 + b*256 + a;
        float f1;
        memcpy(&f1, &ii, sizeof(float));
        
        float f2 = m_values[i];
        if (abs(f1-f2)>0.0001) {
            printf("diff pos[%d] val[%f, %f, %f]", i, f1, total, f2);
        }
    }
    free(rgba);
     
}

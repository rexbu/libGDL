/**
 * file :	GPUConvShape.cpp
 * author :	Rex
 * create :	2017-07-27 18:48
 * func : 
 * history:
 */

#include "GPUConvShape.h"

static const char* g_conv_shpae_vertex = SHADER30_STRING(
    in vec4 position;
    in vec4 inputTextureCoordinate;
    out vec2 textureCoordinate;
    in vec4 outTextureCoordinate;
    out vec2 outCoordinate;
    void main()
    {
        gl_Position = position;
        textureCoordinate = inputTextureCoordinate.xy;
        outCoordinate = outTextureCoordinate.xy;
    }
);

static const char* g_conv_shpae_fragment = SHADER30_STRING(
    // 纹理坐标，用于获取颜色
    in highp vec2 textureCoordinate;
    // 输出坐标，用于计算属于第几个通道
    in highp vec2 outCoordinate;
    out highp vec4 out_color;
    uniform highp sampler2D inputImageTexture[1];
    uniform int channel_count;
                                                           
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
        highp vec4 color = texture(inputImageTexture[0], textureCoordinate);
        int c_i = int(outCoordinate.x*float(channel_count));
        highp float channel[4];
        channel[0] = color.r;
        channel[1] = color.g;
        channel[2] = color.b;
        channel[3] = color.a;
        out_color = encodeFloat(channel[c_i]*255.0);
     }
);

GPUConvShape::GPUConvShape(int normal_size, int channel_count, int next_padding):
m_channel_count(channel_count),
m_next_padding(next_padding),
GPULayerBase(channel_count, "ConvShape"){
    m_inputs = 1;
    m_program = new GPUProgram(g_conv_shpae_vertex, g_conv_shpae_fragment, "GPUConvShape");
    init();
    m_out_coordinate = m_program->attributeIndex("outTextureCoordinate");
    setInteger("channel_count", channel_count);
    m_normal_size = normal_size;
    m_next_padding = next_padding;
    m_fill_mode = GPUFillModePreserveAspectRatio;
    m_outbuffer = new ConvFrameBuffer(normal_size, normal_size, m_next_padding, channel_count, 1);
    setOutputSize(m_outbuffer->m_width, m_outbuffer->m_height);
}

void GPUConvShape::newFrame(){
    render();
    unlockInputFrameBuffers();
    // print();
}
void GPUConvShape::render(){
    GPUContext* context = GPUContext::shareInstance();
    context->glContextLock();   // 加锁，防止此时设置参数
    GPUCheckGlError(m_filter_name.c_str(), true, false);
    
    context->setActiveProgram(m_program);
    activeOutFrameBuffer();
    m_coordinate_buffer->activeBuffer(m_input_coordinate);
    m_vertex_buffer->activeBuffer(m_position);
    m_outcoor_buffer->activeBuffer(m_out_coordinate);
    GPUCheckGlError(m_filter_name.c_str(), true, false);
    for (int i=0; i<m_inputs; i++) {
        m_input_buffers[i]->activeTexture(GL_TEXTURE0+i);
        glUniform1i(m_input_textures[i], 0+i);
    }
    
    glDrawArrays(GL_TRIANGLES, 0, m_channel_count*6);
    glFlush();
    m_coordinate_buffer->disableBuffer(m_input_coordinate);
    m_vertex_buffer->disableBuffer(m_position);
    m_outcoor_buffer->disableBuffer(m_out_coordinate);
    m_outbuffer->unactive();
    
    GPUCheckGlError(m_filter_name.c_str(), true, false);
    context->glContextUnlock();
}

void GPUConvShape::activeOutFrameBuffer(){
    if (m_coordinate_buffer == NULL) {
        memset(m_clear_color, 0, sizeof(float)*4);
        
        m_coordinate_buffer = new GPUVertexBuffer(m_channel_count*6);
        m_coordinate_buffer->setBuffer(&m_coordinates[0]);
        m_vertex_buffer = new GPUVertexBuffer(m_channel_count*6);
        m_vertex_buffer->setBuffer(&m_vertices[0]);
        m_outcoor_buffer = new GPUVertexBuffer(m_channel_count*6);
        m_outcoor_buffer->setBuffer(&m_out_coordinates[0]);
        
    }
    m_outbuffer->activeBuffer();
    
    glClearColor(m_clear_color[0], m_clear_color[1], m_clear_color[2], m_clear_color[3]);
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

void GPUConvShape::calAspectRatio(){
    if (m_out_width==0||m_out_height==0||m_frame_width==0||m_frame_height==0)
    {
        memcpy(&m_vertices[0], g_vertices, sizeof(GLfloat)*8);
        return;
    }
    
    m_vertices.clear();
    m_coordinates.clear();
    m_out_coordinates.clear();
    // 使用GL_TRIANGLES绘制，每个矩形由2个三角形组成，每个通道都用当前图片。gles3.0不支持GL_CLAMP_TO_BORDER，所以只能调整顶点
    float w_channel_step = 2.0*m_normal_size/m_out_width;
    float w_padding_step = 2.0*m_next_padding/m_out_width;
    float h_channel_step = 2.0*m_normal_size/m_out_height;
    float h_padding_step = 2.0*m_next_padding/m_out_height;
    
    float cw_channel_step = 1.0*m_normal_size/m_out_width;
    float cw_padding_step = 1.0*m_next_padding/m_out_width;
    float ch_channel_step = 1.0*m_normal_size/m_out_height;
    float ch_padding_step = 1.0*m_next_padding/m_out_height;
    
    if (m_frame_width>m_frame_height) {
        float frame_ratio = m_frame_height*1.0/m_frame_width;
        // 按照宽度压缩，上下填黑，顶点坐标处于去掉padding还要去掉填黑的区域。使用GL_TRIANGLES绘制，每个矩形由2个三角形共6个顶点组成
        for (int w=0; w<m_channel_count; w++) {
            float lt_x = -1.0 + (w_channel_step+w_padding_step)*w + w_padding_step;
            float lt_y = -1.0 + h_padding_step + (1.0-frame_ratio)*h_channel_step/2.0;
            float rt_x = lt_x + w_channel_step;
            float rt_y = lt_y;
            float lb_x = lt_x;
            float lb_y = 1.0- (1.0-frame_ratio)*h_channel_step/2.0;
            float rb_x = rt_x;
            float rb_y = lb_y;
            float triangle[] = {lt_x, lt_y, rt_x, rt_y, lb_x, lb_y, rt_x, rt_y, lb_x, lb_y, rb_x, rb_y};
            for (int i=0; i<sizeof(triangle)/sizeof(float); i++) {
                m_vertices.push_back(triangle[i]);
            }
            // 输出坐标
            lb_x = (cw_channel_step + cw_padding_step)*w + cw_padding_step;
            lb_y = (1.0-frame_ratio)*ch_channel_step/2.0;
            rb_x = lb_x + cw_channel_step;
            rb_y = lb_y;
            lt_x = lb_x;
            // 需要减掉2倍的黑填充框
            lt_y = 1.0 - ch_padding_step - (1.0-frame_ratio)*ch_channel_step;
            rt_x = rb_x;
            rt_y = lt_y;
            float out_triangle[] = {lb_x, lb_y, rb_x, rb_y, lt_x, lt_y, rb_x, rb_y, lt_x, lt_y, rt_x, rt_y};
            // 第一个三角形坐标
            for (int i=0; i<sizeof(out_triangle)/sizeof(float); i++) {
                m_out_coordinates.push_back(out_triangle[i]);
            }
        }
    }
    else{
        float frame_ratio = m_frame_width*1.0/m_frame_height;
        // 使用GL_TRIANGLES绘制，每个矩形由2个三角形共6个顶点组成
        for (int w=0; w<m_channel_count; w++) {
            float lt_x = -1.0 + (w_channel_step+w_padding_step)*w + w_padding_step + (1.0-frame_ratio)*w_channel_step/2.0;
            float lt_y = -1.0 + h_padding_step;
            // 需要减掉2倍的黑填充框
            float rt_x = lt_x + w_channel_step - (1.0-frame_ratio)*w_channel_step;
            float rt_y = lt_y;
            float lb_x = lt_x;
            float lb_y = lt_y + h_channel_step;
            float rb_x = rt_x;
            float rb_y = lb_y;
            float triangle[] = {lt_x, lt_y, rt_x, rt_y, lb_x, lb_y, rt_x, rt_y, lb_x, lb_y, rb_x, rb_y};
            for (int i=0; i<sizeof(triangle)/sizeof(float); i++) {
                m_vertices.push_back(triangle[i]);
            }
            
            // 输出坐标
            lb_x = (cw_channel_step+cw_padding_step)*w + cw_padding_step + (1.0-frame_ratio)*cw_channel_step/2.0;
            lb_y = 0.0;
            // 需要减掉2倍的黑填充框
            rb_x = lb_x + cw_channel_step - (1.0-frame_ratio)*cw_channel_step;
            rb_y = lb_y;
            lt_x = lb_x;
            lt_y = 1.0 - ch_padding_step;
            rt_x = rb_x;
            rt_y = lt_y;
            float out_triangle[] = {lb_x, lb_y, rb_x, rb_y, lt_x, lt_y, rb_x, rb_y, lt_x, lt_y, rt_x, rt_y};
            // 第一个三角形坐标
            for (int i=0; i<sizeof(out_triangle)/sizeof(float); i++) {
                m_out_coordinates.push_back(out_triangle[i]);
            }
        }
    }
    for (int i=0; i<m_channel_count; i++) {
        const GLfloat* coor = coordinatesRotation(GPUNoRotation);
        float triangle[] = {coor[0], coor[1], coor[2], coor[3], coor[4], coor[5], coor[2], coor[3], coor[4], coor[5], coor[6], coor[7]};
        // 第一个三角形顶点
        for (int i=0; i<sizeof(triangle)/sizeof(float); i++) {
            m_coordinates.push_back(triangle[i]);
        }
    }
}

void GPUConvShape::print(){
    printf("##### %d/%d ####\n", m_outbuffer->m_width, m_outbuffer->m_height);
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
            printf("%d\t", int(f));
        }
        printf("\n");
    }
    
    free(rgba);
}

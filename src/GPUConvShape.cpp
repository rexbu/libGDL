/**
 * file :	GPUConvShape.cpp
 * author :	Rex
 * create :	2017-07-27 18:48
 * func : 
 * history:
 */

#include "GPUConvShape.h"

static const char* g_conv_shpae_fragment = SHADER30_STRING(
    // 纹理坐标，用于获取颜色
    in highp vec2 textureCoordinate;
    out highp uvec4 out_color;
    uniform highp sampler2D inputImageTexture[1];
    uniform float channel[4];
                                                           
#define encodeFloats(v) uvec4(floatBitsToUint((v).r), floatBitsToUint((v).g), floatBitsToUint((v).b), floatBitsToUint((v).a))
#define decodeFloats(v) vec4(uintBitsToFloat((v).r), uintBitsToFloat((v).g), uintBitsToFloat((v).b), uintBitsToFloat((v).a))
                                                           
    void main(){
        highp vec4 color = texture(inputImageTexture[0], textureCoordinate)*vec4(channel[0],channel[1],channel[2],channel[3]);
        out_color = encodeFloats(color);
     }
);

GPUConvShape::GPUConvShape(int normal_size, int channel_count, int next_padding):
m_channel_count(channel_count),
m_next_padding(next_padding),
GPULayerBase(channel_count, "ConvShape"){
    m_inputs = 1;
    m_program = new GPUProgram(g_vertext30_shader, g_conv_shpae_fragment, "GPUConvShape");
    init();
    switch (channel_count) {
        case 1:{
            float channel[4] = {255.0, 0.0, 0.0, 0.0};
            setFloatv("channel", channel, 4);
        }
            break;
        case 2:{
            float channel[4] = {255.0, 255.0, 0.0, 0.0};
            setFloatv("channel", channel, 4);
        }
            break;
        case 3:{
            float channel[4] = {255.0, 255.0, 255.0, 0.0};
            setFloatv("channel", channel, 4);
        }
            break;
        default:
            break;
    }
    
    m_normal_size = normal_size;
    m_next_padding = next_padding;
    m_fill_mode = GPUFillModePreserveAspectRatio;
    setOutputSize(m_normal_size+2*m_next_padding, m_normal_size+2*m_next_padding);
}

void GPUConvShape::activeOutFrameBuffer(){
    if (m_coordinate_buffer == NULL) {
        memset(m_clear_color, 0, sizeof(float)*4);
        
        m_coordinate_buffer = new GPUVertexBuffer();
        m_coordinate_buffer->setBuffer(&m_coordinates[0]);
        m_vertex_buffer = new GPUVertexBuffer();
        m_vertex_buffer->setBuffer(&m_vertices[0]);
        
    }
    m_outbuffer = ConvBufferCache::getFrameBuffer(m_normal_size, m_normal_size, m_next_padding, m_channel_count, 1);
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
        float lt_x = -1.0 + w_padding_step;
        float lt_y = -1.0 + h_padding_step + (1.0-frame_ratio)*h_channel_step/2.0;
        float rt_x = lt_x + w_channel_step;
        float rt_y = lt_y;
        float lb_x = lt_x;
        float lb_y = 1.0- (1.0-frame_ratio)*h_channel_step/2.0;
        float rb_x = rt_x;
        float rb_y = lb_y;
        float triangle[] = {lt_x, lt_y, rt_x, rt_y, lb_x, lb_y, rb_x, rb_y};
        for (int i=0; i<sizeof(triangle)/sizeof(float); i++) {
            m_vertices.push_back(triangle[i]);
        }
    }
    else{
        float frame_ratio = m_frame_width*1.0/m_frame_height;
        // 使用GL_TRIANGLES绘制，每个矩形由2个三角形共6个顶点组成
        float lt_x = -1.0 + w_padding_step + (1.0-frame_ratio)*w_channel_step/2.0;
        float lt_y = -1.0 + h_padding_step;
        // 需要减掉2倍的黑填充框
        float rt_x = lt_x + w_channel_step - (1.0-frame_ratio)*w_channel_step;
        float rt_y = lt_y;
        float lb_x = lt_x;
        float lb_y = lt_y + h_channel_step;
        float rb_x = rt_x;
        float rb_y = lb_y;
        float triangle[] = {lt_x, lt_y, rt_x, rt_y, lb_x, lb_y, rb_x, rb_y};
        for (int i=0; i<sizeof(triangle)/sizeof(float); i++) {
            m_vertices.push_back(triangle[i]);
        }
    }
    const GLfloat* coor = coordinatesRotation(GPUNoRotation);
    for (int i=0; i<8; i++) {
        m_coordinates.push_back(coor[i]);
    }
}

void GPUConvShape::print(){
    printf("##### %d/%d ####\n", m_outbuffer->m_width, m_outbuffer->m_height);
    float* rgba = (float*)m_outbuffer->getPixels();
    for (int h=2; h<m_outbuffer->m_height; h++) {
        for (int w=2; w<m_outbuffer->m_width; w++) {
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

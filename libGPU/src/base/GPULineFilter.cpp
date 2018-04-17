/**
 * file :	GPULineFilter.cpp
 * author :	Rex
 * email : rex@labjk.com
 * create :	2017-05-19 15:20
 * func : 
 * history:
 */

#include "GPULineFilter.h"

const char* g_liner_vertex_shader = SHADER_STRING(
    attribute vec4 position;
    attribute vec4 color;
    varying vec4 lineColor;
    void main()
    {
        gl_Position = position;
        lineColor = color;
    }
);

const char* g_line_fragment_shader = SHADER_STRING(
    varying vec4 lineColor;
    void main()
    {
        gl_FragColor = lineColor;
    }
);

GPULineFilter::GPULineFilter(int w, int h):GPUFilter(false, "GPULineFilter"){
    m_program = new GPUProgram(g_liner_vertex_shader, g_line_fragment_shader);
    m_inputs = 0;
    init();
    // initParams函数会初始化m_out_width和m_out_height
    m_out_width = w;
    m_out_height = h;
    m_line_width = 1;
}

void GPULineFilter::initShader(){
    if (m_program==NULL)
    {
        err_log("Program create Error!");
        return;
    }
    
    GPUContext* context = GPUContext::shareInstance();
    context->glContextLock();
    
    m_position = m_program->attributeIndex("position");
    m_color = m_program->attributeIndex("color");
    glEnableVertexAttribArray(m_position);
    glEnableVertexAttribArray(m_color);
    
    m_program->link();
    context->glContextUnlock();
    GPUCheckGlError("Filter init");
}

void GPULineFilter::render(){
#if DEBUG_FILTER_NAME
    err_log("filter name: %s texture: %d", m_filter_name.c_str(), m_input_buffers[0]->m_texture);
#endif
    
    gpu_size_t size = sizeOfFBO();
    size.width /= 2;
    size.height /= 2;
    // 先画端点
    for (int v=0; v<m_vertex_count-1; v++) {
        // 画圆
        drawRound(m_points[v*2], m_points[v*2+1], m_line_width, &m_colors[v*4]);
        // 画矩形线条
        // 起点端点坐标(x0-r*sin, y0+r*cos), (x0+r*sin, y0-r*cos)
        // 终点端点坐标(x1-r*sin, y1+r*cos), (x1+r*sin, y1-r*cos)
        // sin = (y1-y0)/d, cos = (x1-x0)/d
        float x0 = m_points[v*2];
        float y0 = m_points[v*2+1];
        float x1 = m_points[v*2+2];
        float y1 = m_points[v*2+3];
        float d = sqrt(pow(x1-x0, 2)+pow(y1-y0, 2));
        float sin = (y1-y0)/d;
        float cos = (x1-x0)/d;
        float r = m_line_width;
        
        float vertex[8];
        vertex[0] = x0-r*sin;
        vertex[1] = y0+r*cos;
        vertex[2] = x0+r*sin;
        vertex[3] = y0-r*cos;
        
        vertex[4] = x1-r*sin;
        vertex[5] = y1+r*cos;
        vertex[6] = x1+r*sin;
        vertex[7] = y1-r*cos;
        // 归一化
        for (int i=0; i<4; i++) {
            vertex[i*2] = (vertex[i*2] - size.width) / size.width;
            vertex[i*2+1] = (vertex[i*2+1] - size.height) / size.height;
        }
        
        GPUVertexBuffer* vertex_buffer = GPUVertexBufferCache::shareInstance()->getVertexBuffer(4);
        vertex_buffer->activeBuffer(m_position, vertex);
        
        GLfloat colors[4*4];
        memcpy(colors, &m_colors[v*4], sizeof(GLfloat)*4);
        memcpy(colors+4, &m_colors[v*4], sizeof(GLfloat)*4);
        memcpy(colors+8, &m_colors[(v+1)*4], sizeof(GLfloat)*4);
        memcpy(colors+12, &m_colors[(v+1)*4], sizeof(GLfloat)*4);
        GPUVertexBuffer* color_buffer = GPUVertexBufferCache::shareInstance()->getVertexBuffer(4, 4);
        color_buffer->activeBuffer(m_color, colors);
        
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        
        vertex_buffer->disableBuffer(m_position);
        color_buffer->disableBuffer(m_color);
        
        vertex_buffer->unLock();
        color_buffer->unLock();
    }
    
    // 画最后一个端点
    drawRound(m_points[(m_vertex_count-1)*2], m_points[(m_vertex_count-1)*2+1], m_line_width, &m_colors[(m_vertex_count-1)*4]);
    
//    GPUVertexBuffer* vertex_buffer = GPUVertexBufferCache::shareInstance()->getVertexBuffer(m_vertex_count);
//    vertex_buffer->activeBuffer(m_position, &m_points[0]);
//    
//    GPUVertexBuffer* color_buffer = GPUVertexBufferCache::shareInstance()->getVertexBuffer(m_vertex_count, 4);
//    color_buffer->activeBuffer(m_color, &m_colors[0]);
//    
//    glDrawArrays(GL_LINE_STRIP, 0, m_vertex_count);
    glFlush();
}

void GPULineFilter::drawPoint(float x, float y, uint32_t radius, float* color){
    GLfloat vertices[720];
    GLfloat colors[360*4];
    // 颜色
    for (int i=0; i<360; i++) {
        memcpy(colors+i*4, color, sizeof(float)*4);
    }
    
    gpu_size_t size = sizeOfFBO();
    size.width /= 2;
    size.height /= 2;
    
    for (int i=0; i < 360; i++) {
        GLfloat radians = M_PI * i / 180.0;
        vertices[i*2] = ((x + cos(radians) * radius) - size.width) / size.width;
        vertices[i*2+1] = ((y + sin(radians) * radius) - size.height) / size.height;
    }
    
    GPUVertexBuffer* vertex_buffer = GPUVertexBufferCache::shareInstance()->getVertexBuffer(360);
    vertex_buffer->activeBuffer(m_position, vertices);
    GPUVertexBuffer* color_buffer = GPUVertexBufferCache::shareInstance()->getVertexBuffer(360, 4);
    color_buffer->activeBuffer(m_color, colors);
    
    glDrawArrays(GL_TRIANGLE_FAN, 0, 360);
    glFlush();
    
    vertex_buffer->unLock();
    color_buffer->unLock();
}

// 360等份画扇形画圆
void GPULineFilter::drawRound(float x, float y, uint32_t radius, float* color){
    GLfloat vertices[722];
    GLfloat colors[361*4];
    // 颜色
    for (int i=0; i<361; i++) {
        memcpy(colors+i*4, color, sizeof(float)*4);
    }
    
    gpu_size_t size = sizeOfFBO();
    size.width /= 2;
    size.height /= 2;
    
    // 361个顶点坐标
    vertices[0] = (x-size.width)/size.width;
    vertices[1] = (y - size.height)/size.height;
    for (int i=1; i < 361; i++) {
        GLfloat radians = M_PI * (i-1) / 180.0;
        vertices[i*2] = ((x + cos(radians) * radius) - size.width) / size.width;
        vertices[i*2+1] = ((y + sin(radians) * radius) - size.height) / size.height;
    }
    
    GPUVertexBuffer* vertex_buffer = GPUVertexBufferCache::shareInstance()->getVertexBuffer(361);
    vertex_buffer->activeBuffer(m_position, vertices);
    GPUVertexBuffer* color_buffer = GPUVertexBufferCache::shareInstance()->getVertexBuffer(361, 4);
    color_buffer->activeBuffer(m_color, colors);
    
    glDrawArrays(GL_TRIANGLE_FAN, 0, 361);
    glFlush();
    
    vertex_buffer->unLock();
    color_buffer->unLock();
}

void GPULineFilter::setPoints(gpu_point_t* points, int n){
    m_points.resize(n*2);
    m_vertex_count = n;
    
//    gpu_size_t size = sizeOfFBO();
//    size.width /= 2;
//    size.height /= 2;
//    
//    // 设置线段顶点，顶点的坐标范围为[-1， 1]
//    for (int i=0; i<n; i++) {
//        m_points[2*i] = (points[i].x-size.width)/(size.width);
//        m_points[2*i+1] = (points[i].y-size.height)/(size.height);
//    }
    // 不做归一化，归一化在render函数画矩形时候执行
    for (int i=0; i<n; i++) {
        m_points[2*i] = points[i].x;
        m_points[2*i+1] = points[i].y;
    }
    
    if (m_colors.size()==0) {
        m_colors.resize(n*4);
        float color[4] = {0.0, 0.0, 0.0, 1.0};
        for (int i=0; i<n; i++) {
            memcpy(&m_colors[i*4], color, sizeof(float)*4);
        }
    }
}

void GPULineFilter::setLineWidth(uint32_t width){
    m_line_width = width;
}

void GPULineFilter::setColors(gpu_colorf_t* color, int n){
    m_colors.resize(n*4);
    for (int i=0; i<n; i++) {
        m_colors[i*4] = color[i].r;
        m_colors[i*4+1] = color[i].g;
        m_colors[i*4+2] = color[i].b;
        m_colors[i*4+3] = color[i].a;
    }
}

void GPULineFilter::setColor(gpu_colorf_t* color, int i){
    m_colors[i*4] = color->r;
    m_colors[i*4+1] = color->g;
    m_colors[i*4+2] = color->b;
    m_colors[i*4+3] = color->a;
}

/**
 * file :	GPUFilter.cpp
 * author :	Rex
 * email : rex@labjk.com
 * create :	2016-04-27 19:12
 * func : 
 * history:
 */
#include "GPUFilter.h"
#include "GPUVertexBuffer.h"

#pragma --mark "全局变量的定义"
const char* GPUFilter::g_vertext30_shader = SHADER30_STRING(
    in vec4 position;
    in vec4 inputTextureCoordinate;
    out vec2 textureCoordinate;
    void main()
    {
        gl_Position = position;
        textureCoordinate = inputTextureCoordinate.xy;
    }
);
const char* GPUFilter::g_fragment30_shader = SHADER30_STRING(
    in vec2 textureCoordinate;
    uniform sampler2D inputImageTexture[1];
    out vec4 out_color;
    void main()
    {
        out_color = fract(texture(inputImageTexture[0], textureCoordinate));
    }
);
const char* GPUFilter::g_fragment_shader = SHADER_STRING(
  varying vec2 textureCoordinate;
  uniform sampler2D inputImageTexture[1];

  void main()
  {
        gl_FragColor = texture2D(inputImageTexture[0], textureCoordinate);
  }
);

const GLfloat GPUFilter::g_vertices[] = {
    -1.0f, -1.0f,
    1.0f, -1.0f,
    -1.0f,  1.0f,
    1.0f,  1.0f,
};

#pragma --mark "构造与析构函数"
GPUFilter::GPUFilter(bool compile, const char* name){
    if (name==NULL) {
        m_filter_name = "GPUFilter";
    }
    else{
        m_filter_name = name;
    }

    if (compile) {
        m_program = new GPUProgram(g_vertext_shader[0], g_fragment_shader, m_filter_name.c_str());
        init();
    }
    else{
        m_program = NULL;
        initParams();
    }
}

GPUFilter::GPUFilter(const char* fragment, int inputs, const char* name):
GPUInput(inputs){
    if (name==NULL) {
        m_filter_name = "GPUFilter";
    }
    else{
        m_filter_name = name;
    }

	m_program = new GPUProgram(g_vertext_shader[inputs-1], fragment, m_filter_name.c_str());
    init();
}

GPUFilter::GPUFilter(const char* vertext, const char* fragment, int inputs):
GPUInput(inputs){
	m_program = new GPUProgram(vertext, fragment);
    init();
}

GPUFilter::~GPUFilter()
{
    if (m_program)
    {
		delete m_program;
        m_program = NULL;
    }
    if (m_coordinate_buffer!=NULL) {
        delete m_coordinate_buffer;
    }
    if (m_vertex_buffer!=NULL) {
        delete m_vertex_buffer;
    }
}
#pragma --mark "初始化"
void GPUFilter::changeShader(const char* fragment, const char* vertex){
    if (fragment==NULL) {
        return;
    }
    const char* ver = vertex;
    if (ver==NULL) {
        ver = g_vertext_shader[0];
    }
    
    GPUContext* context = GPUContext::shareInstance();
    context->glContextLock();
    
    context->makeCurrent();
    delete m_program;
    context->glContextUnlock();
    
    m_program = new GPUProgram(ver, fragment);
    
    initShader();
}

void GPUFilter::init(){
   initParams();
   initShader();
}

void GPUFilter::initParams(){
    m_out_width = 0;
    m_out_height = 0;
    m_frame_width = 0;
    m_frame_height = 0;
    m_outbuffer = NULL;
    m_coordinate_buffer = NULL;
    m_vertex_buffer = NULL;
    m_special_outbuffer = NULL;
    m_option = NULL;
    m_clear_color[0] = 1.0f;
    m_clear_color[1] = 1.0f;
    m_clear_color[2] = 1.0f;
    m_clear_color[3] = 1.0f;
    m_rotation = GPUNoRotation;
    m_coordinates.resize(8);
    m_coordinates[0] = -1;
    // 初始化顶点数据
    m_vertices.resize(8);
    memcpy(&m_vertices[0], g_vertices, sizeof(GLfloat)*8);
    
    setFillMode(GPUFillModeStretch);
}
void GPUFilter::initShader(){
    if (m_program==NULL)
    {
        err_log("Program create Error!");
        return;
    }
    
    GPUContext* context = GPUContext::shareInstance();
    context->glContextLock();
    
    setInputs(m_inputs);
    
    m_position = m_program->attributeIndex("position");
    m_input_coordinate = m_program->attributeIndex("inputTextureCoordinate");
    // 默认纹理坐标
    m_coordinates[0] = -1;
    
    // 目前fragment最多支持16个纹理, setInputs函数已经申请了m_input_textures空间
    for (int i=0; i<m_inputs; i++) {
        // 有可能和其他变量冲突
        m_input_textures[i] = i;
    }
    m_program->setUniformsIndex("inputImageTexture", &m_input_textures[0], m_inputs);
    m_input_texture = m_input_textures[0];
    glEnableVertexAttribArray(m_position);
    glEnableVertexAttribArray(m_input_coordinate);
    
    m_program->link();
    context->glContextUnlock();
    GPUCheckGlError("Filter init");
}

void GPUFilter::setInputs(int inputs){
    GPUInput::setInputs(inputs);
    m_input_textures.resize(inputs);
    m_input_coordinate = -1;
    for (int i = 0; i < inputs; ++i)
    {
        m_input_textures[i] = -1;
    }
}

#pragma --mark "渲染"
void GPUFilter::render(){
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
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)m_vertices.size()/2);
    glFlush();
    m_coordinate_buffer->disableBuffer(m_input_coordinate);
    m_vertex_buffer->disableBuffer(m_position);
    m_outbuffer->unactive();
    
    GPUCheckGlError(m_filter_name.c_str(), true, false);
    context->glContextUnlock();
}

void GPUFilter::newFrame(){
    if (ready())
    {
        if (m_disable) {
            m_outbuffer = m_input_buffers[0];
            unlockInputFrameBuffers();
            informTargets();
        }
        else{
            render();
            m_special_outbuffer = NULL;
            
            // render后的回调
            if (m_complete!=NULL) {
                m_complete(this, m_para);
            }
            
            unlockInputFrameBuffers();
            informTargets();
        }
    }
}

void GPUFilter::activeOutFrameBuffer(){
    if (m_special_outbuffer==NULL) {
        if (m_option==NULL) {
            m_outbuffer = GPUBufferCache::shareInstance()->getFrameBuffer(sizeOfFBO(), false);
        }
        else{
            m_outbuffer = GPUBufferCache::shareInstance()->getFrameBuffer(sizeOfFBO(), m_option, false);
        }
        if (m_vertex_buffer == NULL) {
            m_coordinate_buffer = new GPUVertexBuffer();
            m_vertex_buffer = new GPUVertexBuffer();
            m_vertex_buffer->setBuffer(&m_vertices[0]);
            // 更新纹理坐标
            if (m_coordinates[0]==-1) {
                memcpy(&m_coordinates[0], GPUFilter::coordinatesRotation(m_rotation), sizeof(GLfloat)*8);
            }
        }
        // 有可能中途切换镜像等操作，每次设置一次
        m_coordinate_buffer->setBuffer(&m_coordinates[0]);
        m_outbuffer->activeBuffer();
        
        glClearColor(m_clear_color[0], m_clear_color[1], m_clear_color[2], m_clear_color[3]);
        glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    }
    else{
        m_outbuffer = m_special_outbuffer;
        m_outbuffer->activeBuffer();
    }
}

gpu_size_t GPUFilter::sizeOfFBO(){
    gpu_size_t size = {0, 0};
    if (m_out_width!=0 && m_out_height!=0) {
        size.width = m_out_width;
        size.height = m_out_height;
	}
    else{
        size.width = m_frame_width;
        size.height = m_frame_height;
    }

    return size;
}

#pragma --mark "设置项"
void GPUFilter::setOutputSize(uint32_t width, uint32_t height){
    GPUOutput::setOutputSize(width, height);
    calAspectRatio();
}

void GPUFilter::setFillMode(gpu_fill_mode_t mode){
    m_fill_mode = mode;
    calAspectRatio();
}

void GPUFilter::setFrameSize(uint32_t width, uint32_t height){
    GPUInput::setFrameSize(width, height);
    calAspectRatio();
}

void GPUFilter::calAspectRatio(){
    switch(m_fill_mode){
        // 用黑框填充
        case GPUFillModePreserveAspectRatio:
            if (m_out_width==0||m_out_height==0||m_frame_width==0||m_frame_height==0)
            {
                memcpy(&m_vertices[0], g_vertices, sizeof(GLfloat)*8);
            }
            else{
                float frame_ratio = m_frame_width*1.0/m_frame_height;
                float out_ratio = m_out_width*1.0/m_out_height;
                if (frame_ratio>out_ratio) {
                    // 按照宽度压缩，上下填黑
                    float w_ratio = m_out_width*1.0/m_frame_width;
                    float normal_h = w_ratio*m_frame_height;
                    m_vertices[0] = -1.0;
                    m_vertices[1] = (-1.0)*normal_h/m_out_height;
                    m_vertices[2] = 1.0;
                    m_vertices[3] = m_vertices[1];
                    m_vertices[4] = -1.0;
                    m_vertices[5] = normal_h/m_out_height;
                    m_vertices[6] = 1.0;
                    m_vertices[7] = m_vertices[5];
                }
                else{
                    // 按照高度压缩，左右填黑
                    float h_ratio = m_out_height*1.0/m_frame_height;
                    float normal_w = h_ratio*m_frame_width;
                    m_vertices[0] = (-1.0)*normal_w/m_out_width;
                    m_vertices[1] = -1.0;
                    m_vertices[2] = normal_w/m_out_width;
                    m_vertices[3] = -1.0;
                    m_vertices[4] = m_vertices[0];
                    m_vertices[5] = 1.0;
                    m_vertices[6] = m_vertices[2];
                    m_vertices[7] = 1.0;
                }
            }
            break;
        // 按照比例裁剪
        case GPUFillModePreserveAspectRatioAndFill:
            if (m_out_width==0||m_out_height==0||m_frame_width==0||m_frame_height==0)
            {
                memcpy(&m_vertices[0], g_vertices, sizeof(GLfloat)*8);
            }
            else{
                float wh_ratio = m_out_width*1.0/m_out_height;
                float hw_ratio = m_out_height*1.0/m_out_width;
                if (wh_ratio > m_frame_width*1.0/m_frame_height) {
                    float ratioy = m_frame_width*hw_ratio;
                    m_vertices[0] = -1.0;
                    m_vertices[1] = (-1.0)*m_frame_height/ratioy;
                    m_vertices[2] = 1.0;
                    m_vertices[3] = m_vertices[1];
                    m_vertices[4] = -1.0;
                    m_vertices[5] = m_frame_height/ratioy;
                    m_vertices[6] = 1.0;
                    m_vertices[7] = m_vertices[5];
                }
                else{
                    float ratiox = m_frame_height*wh_ratio;
                    m_vertices[0] = (-1.0)*m_frame_width/ratiox;
                    m_vertices[1] = -1.0;
                    m_vertices[2] = m_frame_width/ratiox;
                    m_vertices[3] = -1.0;
                    m_vertices[4] = m_vertices[0];
                    m_vertices[5] = 1.0;
                    m_vertices[6] = m_vertices[2];
                    m_vertices[7] = 1.0;
                }
            }
            break;
        // 直接拉伸
        case GPUFillModeStretch:
        default:
            memcpy(&m_vertices[0], g_vertices, sizeof(GLfloat)*8);
            break;
    }
    // err_log("filter: %s, fill: %d", m_filter_name.c_str(), m_fill_mode);
    // err_log("vertex: %0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f", 
    //         m_vertices[0],m_vertices[1],m_vertices[2],m_vertices[3],m_vertices[4],m_vertices[5],m_vertices[6],m_vertices[7]);
}

const GLfloat* GPUFilter::coordinatesRotation(gpu_rotation_t mode)
{
    static const GLfloat noRotationTextureCoordinates[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
    };
    
    static const GLfloat rotateLeftTextureCoordinates[] = {
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,
    };
    
    static const GLfloat rotateRightTextureCoordinates[] = {
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
    };
    
    static const GLfloat verticalFlipTextureCoordinates[] = {
        0.0f, 1.0f,
        1.0f, 1.0f,
        0.0f,  0.0f,
        1.0f,  0.0f,
    };
    
    static const GLfloat horizontalFlipTextureCoordinates[] = {
        1.0f, 0.0f,
        0.0f, 0.0f,
        1.0f,  1.0f,
        0.0f,  1.0f,
    };
    
    static const GLfloat rotateRightVerticalFlipTextureCoordinates[] = {
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
    };
    
    static const GLfloat rotateRightHorizontalFlipTextureCoordinates[] = {
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
    };
    static const GLfloat rotateLeftHorizontalFlipTextureCoordinates[] = {
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
    };
    
    static const GLfloat rotate180TextureCoordinates[] = {
        1.0f, 1.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
    };
    
    switch(mode)
    {
        case GPUNoRotation: return noRotationTextureCoordinates;
        case GPURotateLeft: return rotateLeftTextureCoordinates;
        case GPURotateRight: return rotateRightTextureCoordinates;
        case GPUFlipVertical: return verticalFlipTextureCoordinates;
        case GPUFlipHorizonal: return horizontalFlipTextureCoordinates;
        case GPURotateRightFlipVertical: return rotateRightVerticalFlipTextureCoordinates;
        case GPURotateRightFlipHorizontal: return rotateRightHorizontalFlipTextureCoordinates;
        case GPURotate180: return rotate180TextureCoordinates;
		default: return noRotationTextureCoordinates;
    }
}

void GPUFilter::enableAttribArray(const char* name){
    GLuint index = m_program->attributeIndex(name);
    glEnableVertexAttribArray(index);
}

void GPUFilter::setFloat(const char *name, GLfloat val){
    m_program->setFloat(name, val);
}
void GPUFilter::setFloat(const char *name, GLfloat *val, int num){
    m_program->setFloat(name, val, num);
}
void GPUFilter::setFloatv(const char* name, GLfloat* val, int num){
    m_program->setFloatv(name, val, num);
}

void GPUFilter::setMatrix(const char *name, GLfloat *val, int num){
    m_program->setMatrix(name, val, num);
}

void GPUFilter::setInteger(const char* name, int i){
    m_program->setInteger(name, i);
}

const char* GPUFilter::g_vertext_shader[] = {
SHADER_STRING(
    attribute vec4 position;
    attribute vec4 inputTextureCoordinate;
    varying vec2 textureCoordinate;
    void main()
    {
        gl_Position = position;
        textureCoordinate = inputTextureCoordinate.xy;
    }
),
SHADER_STRING(
    attribute vec4 position;
    attribute vec4 inputTextureCoordinate;
    attribute vec4 inputTextureCoordinate2;
    varying vec2 textureCoordinate;
    varying vec2 textureCoordinate2;
    void main()
    {
        gl_Position = position;
        textureCoordinate = inputTextureCoordinate.xy;
        textureCoordinate2 = inputTextureCoordinate2.xy;
    }
),
SHADER_STRING(
    attribute vec4 position;
    attribute vec4 inputTextureCoordinate;
    attribute vec4 inputTextureCoordinate2;
    attribute vec4 inputTextureCoordinate3;
    varying vec2 textureCoordinate;
    varying vec2 textureCoordinate2;
    varying vec2 textureCoordinate3;
    void main()
    {
        gl_Position = position;
        textureCoordinate = inputTextureCoordinate.xy;
        textureCoordinate2 = inputTextureCoordinate2.xy;
        textureCoordinate3 = inputTextureCoordinate3.xy;
    }
),
SHADER_STRING(
    attribute vec4 position;
    attribute vec4 inputTextureCoordinate;
    attribute vec4 inputTextureCoordinate2;
    attribute vec4 inputTextureCoordinate3;
    attribute vec4 inputTextureCoordinate4;
    varying vec2 textureCoordinate;
    varying vec2 textureCoordinate2;
    varying vec2 textureCoordinate3;
    varying vec2 textureCoordinate4;
    void main()
    {
        gl_Position = position;
        textureCoordinate = inputTextureCoordinate.xy;
        textureCoordinate2 = inputTextureCoordinate2.xy;
        textureCoordinate3 = inputTextureCoordinate3.xy;
        textureCoordinate4 = inputTextureCoordinate4.xy;
    }
),
SHADER_STRING(
 attribute vec4 position;
 attribute vec4 inputTextureCoordinate;
 attribute vec4 inputTextureCoordinate2;
 attribute vec4 inputTextureCoordinate3;
 attribute vec4 inputTextureCoordinate4;
 attribute vec4 inputTextureCoordinate5;

 varying vec2 textureCoordinate;
 varying vec2 textureCoordinate2;
 varying vec2 textureCoordinate3;
 varying vec2 textureCoordinate4;
 varying vec2 textureCoordinate5;
 void main()
 {
     gl_Position = position;
     textureCoordinate = inputTextureCoordinate.xy;
     textureCoordinate2 = inputTextureCoordinate2.xy;
     textureCoordinate3 = inputTextureCoordinate3.xy;
     textureCoordinate4 = inputTextureCoordinate4.xy;
     textureCoordinate5 = inputTextureCoordinate5.xy;
 }
),
SHADER_STRING(
 attribute vec4 position;
 attribute vec4 inputTextureCoordinate;
 attribute vec4 inputTextureCoordinate2;
 attribute vec4 inputTextureCoordinate3;
 attribute vec4 inputTextureCoordinate4;
 attribute vec4 inputTextureCoordinate5;
 attribute vec4 inputTextureCoordinate6;

 varying vec2 textureCoordinate;
 varying vec2 textureCoordinate2;
 varying vec2 textureCoordinate3;
 varying vec2 textureCoordinate4;
 varying vec2 textureCoordinate5;
 varying vec2 textureCoordinate6;
 void main()
 {
     gl_Position = position;
     textureCoordinate = inputTextureCoordinate.xy;
     textureCoordinate2 = inputTextureCoordinate2.xy;
     textureCoordinate3 = inputTextureCoordinate3.xy;
     textureCoordinate4 = inputTextureCoordinate4.xy;
     textureCoordinate5 = inputTextureCoordinate5.xy;
     textureCoordinate6 = inputTextureCoordinate6.xy;
 }
),
SHADER_STRING(
 attribute vec4 position;
 attribute vec4 inputTextureCoordinate;
 attribute vec4 inputTextureCoordinate2;
 attribute vec4 inputTextureCoordinate3;
 attribute vec4 inputTextureCoordinate4;
 attribute vec4 inputTextureCoordinate5;
 attribute vec4 inputTextureCoordinate6;
 attribute vec4 inputTextureCoordinate7;

 varying vec2 textureCoordinate;
 varying vec2 textureCoordinate2;
 varying vec2 textureCoordinate3;
 varying vec2 textureCoordinate4;
 varying vec2 textureCoordinate5;
 varying vec2 textureCoordinate6;
 varying vec2 textureCoordinate7;
 void main()
 {
     gl_Position = position;
     textureCoordinate = inputTextureCoordinate.xy;
     textureCoordinate2 = inputTextureCoordinate2.xy;
     textureCoordinate3 = inputTextureCoordinate3.xy;
     textureCoordinate4 = inputTextureCoordinate4.xy;
     textureCoordinate5 = inputTextureCoordinate5.xy;
     textureCoordinate6 = inputTextureCoordinate6.xy;
     textureCoordinate7 = inputTextureCoordinate7.xy;
 }
),
SHADER_STRING(
 attribute vec4 position;
 attribute vec4 inputTextureCoordinate;
 attribute vec4 inputTextureCoordinate2;
 attribute vec4 inputTextureCoordinate3;
 attribute vec4 inputTextureCoordinate4;
 attribute vec4 inputTextureCoordinate5;
 attribute vec4 inputTextureCoordinate6;
 attribute vec4 inputTextureCoordinate7;
 attribute vec4 inputTextureCoordinate8;

 varying vec2 textureCoordinate;
 varying vec2 textureCoordinate2;
 varying vec2 textureCoordinate3;
 varying vec2 textureCoordinate4;
 varying vec2 textureCoordinate5;
 varying vec2 textureCoordinate6;
 varying vec2 textureCoordinate7;
 varying vec2 textureCoordinate8;
 void main()
 {
     gl_Position = position;
     textureCoordinate = inputTextureCoordinate.xy;
     textureCoordinate2 = inputTextureCoordinate2.xy;
     textureCoordinate3 = inputTextureCoordinate3.xy;
     textureCoordinate4 = inputTextureCoordinate4.xy;
     textureCoordinate5 = inputTextureCoordinate5.xy;
     textureCoordinate6 = inputTextureCoordinate6.xy;
     textureCoordinate7 = inputTextureCoordinate7.xy;
     textureCoordinate8 = inputTextureCoordinate8.xy;
 }
),
};

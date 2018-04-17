
/**
 * file :	GPUBuffer.cpp
 * author :	Rex
 * email : rex@labjk.com
 * create :	2016-04-27 19:12
 * func : 
 * history:
 */

#include "GPUFrameBuffer.h"

static gpu_frame_option_t g_frame_default_option = {
    GPU_TEXTURE_RGBA,   // 纹理类别
    GL_TEXTURE_2D,      // 纹理维度
    GL_LINEAR,          // min_filter
    GL_LINEAR,          // max_filter, Mipmap只作用于min_filter, 设置mag_filter的Mipmap选项会导致无效操作，错误码为GL_INVALID_ENUM
    GL_CLAMP_TO_EDGE,   // wrap_s
    GL_CLAMP_TO_EDGE,   // wrap_t
    GL_CLAMP_TO_EDGE,   // wrap_r
    GL_RGBA,            // color_internal_format
    GL_RGBA,            // format
    GL_UNSIGNED_BYTE,   // type
    GL_DEPTH_COMPONENT16,// depth_internal_format
    GL_COLOR_ATTACHMENT0// attachment type
};
static gpu_frame_option_t g_frame_near_option = {
    GPU_TEXTURE_RGBA,   // 纹理类别
    GL_TEXTURE_2D,      // 纹理维度
    GL_NEAREST,          // min_filter
    GL_NEAREST,          // max_filter, Mipmap只作用于min_filter, 设置mag_filter的Mipmap选项会导致无效操作，错误码为GL_INVALID_ENUM
    GL_CLAMP_TO_EDGE,   // wrap_s
    GL_CLAMP_TO_EDGE,   // wrap_t
    GL_CLAMP_TO_EDGE,   // wrap_r
    GL_RGBA,           // color_internal_format
    GL_RGBA,            // format
    GL_UNSIGNED_BYTE,   // type
    GL_DEPTH_COMPONENT16,// depth_internal_format
    GL_COLOR_ATTACHMENT0// attachment type
};
static gpu_frame_option_t g_frame_uint_option = {
    GPU_TEXTURE_RGBA,   // 纹理类别
    GL_TEXTURE_2D,      // 纹理维度
    GL_NEAREST,          // min_filter
    GL_NEAREST,          // max_filter, Mipmap只作用于min_filter, 设置mag_filter的Mipmap选项会导致无效操作，错误码为GL_INVALID_ENUM
    GL_CLAMP_TO_EDGE,   // wrap_s
    GL_CLAMP_TO_EDGE,   // wrap_t
    GL_CLAMP_TO_EDGE,   // wrap_r
    GL_RGBA32UI,           // color_internal_format
    GL_RGBA_INTEGER,    // format
    GL_UNSIGNED_INT,   // type
    GL_DEPTH_COMPONENT16,// depth_internal_format
    GL_COLOR_ATTACHMENT0// attachment type
};
static gpu_frame_option_t g_frame_float_option = {
    GPU_TEXTURE_RGBA,   // 纹理类别
    GL_TEXTURE_2D,      // 纹理维度
    GL_NEAREST,          // min_filter
    GL_NEAREST,          // max_filter, Mipmap只作用于min_filter, 设置mag_filter的Mipmap选项会导致无效操作，错误码为GL_INVALID_ENUM
    GL_CLAMP_TO_EDGE,   // wrap_s
    GL_CLAMP_TO_EDGE,   // wrap_t
    GL_CLAMP_TO_EDGE,   // wrap_r
    GL_RGBA32F,           // color_internal_format
    GL_RGBA,            // format
    GL_FLOAT,           // type
    GL_DEPTH_COMPONENT16,// depth_internal_format
    GL_COLOR_ATTACHMENT0// attachment type
};

#pragma --mark "GPUFrameBuffer"
GPUFrameBuffer::GPUFrameBuffer(gpu_size_t size, bool only_texture)
{
    init(size.width, size.height, 0, &g_frame_default_option, only_texture);
}

GPUFrameBuffer::GPUFrameBuffer(int width, int height, bool only_texture){
    init(width, height, 0, &g_frame_default_option, only_texture);
}

GPUFrameBuffer::GPUFrameBuffer(int width, int height, gpu_frame_option_t* option, bool only_texture){
    init(width, height, 0, option, only_texture);
}
GPUFrameBuffer::GPUFrameBuffer(int width, int height, int depth, gpu_frame_option_t* option, bool only_texture){
    init(width, height, depth, option, only_texture);
}

GPUFrameBuffer::GPUFrameBuffer(int width, int height, GLuint texture, gpu_frame_option_t* option):
m_width(width),
m_height(height){
    m_depth = 0;
    m_texture = texture;
    m_outtexture = true;
    m_framebuffer = 0;
    m_renderbuffer = 0;
    m_referencecount = 0;
    m_is_reference = true;
    if (option==NULL) {
        m_option = GPUFrameBuffer::defaultFrameOption();
    }
    else{
        m_option = *option;
    }
}

void GPUFrameBuffer::init(int width, int height, int depth, gpu_frame_option_t* option, bool only_texture){
    m_width = width;
    m_height = height;
    m_depth = depth;
    m_rgba = NULL;
    m_current_layer = 0;
    m_referencecount = 0;
    memcpy(&m_option, option, sizeof(gpu_frame_option_t));
    if (depth>1) {
        m_option.texture_dim = GL_TEXTURE_3D;
    }
    
    m_is_reference = true;
    
    if (only_texture) {
        generateTexture();
        m_framebuffer = 0;
        m_renderbuffer = 0;
    }
    else{
        generateFrameBuffer();
    }
}

void GPUFrameBuffer::generateTexture(){
    m_outtexture = false;
    m_texture = generateTexture(m_width, m_height, m_depth, &m_option);
}

int GPUFrameBuffer::generateTexture(int width, int height, int depth, gpu_frame_option_t* option){
    GPUContext::shareInstance()->makeCurrent();
    
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    glActiveTexture(GL_TEXTURE1);
    GLuint texture;
    glGenTextures(1, &texture);
    if (depth <= 1) {
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, option->color_format, width, height, 0, option->format, option->type, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, option->wrap_s);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, option->wrap_t);
        // 默认过滤方式是GL_LINEAR_MIPMAP_LINEAR，需要生成贴图所有必要的mips
        if (option->min_filter>=GL_NEAREST_MIPMAP_NEAREST || option->mag_filter>=GL_NEAREST_MIPMAP_NEAREST) {
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    else{
        if (option->format == GL_INVALID_ENUM) {
            err_log("Depth texture can't be 3D_texture!");
            return texture;
        }
        glTexImage3D(GL_TEXTURE_3D, 0, option->color_format, width, height, depth, 0, option->format, option->type, NULL);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, option->wrap_s);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, option->wrap_t);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, option->wrap_t);
        // 默认过滤方式是GL_LINEAR_MIPMAP_LINEAR，需要生成贴图所有必要的mips
        if (option->min_filter>=GL_NEAREST_MIPMAP_NEAREST || option->mag_filter>=GL_NEAREST_MIPMAP_NEAREST) {
            glGenerateMipmap(GL_TEXTURE_3D);
        }
        glBindTexture(GL_TEXTURE_3D, 0);
    }
    
    GPUCheckGlError("generateTexture", true, false);
    
    // fbo过程不能设置filter，在当做sample之前设置
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_texture_option.min_filter);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_texture_option.mag_filter);
    
    return texture;
}
void GPUFrameBuffer::generateFrameBuffer(){
    GPUContext::shareInstance()->makeCurrent();
    GLint defaultFBO;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultFBO);
    
    generateTexture();
    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    
    glGenRenderbuffers(1, &m_renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_renderbuffer);
    // glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24_OES, m_width, m_height);
    glRenderbufferStorage(GL_RENDERBUFFER, m_option.depth_format, m_width, m_height);
    
    // 设置附着点类型，一般颜色渲染使用GL_COLOR_ATTACHMENT0
    if (m_option.texture_dim==GL_TEXTURE_2D) {
        glBindTexture(GL_TEXTURE_2D, m_texture);
        glFramebufferTexture2D(GL_FRAMEBUFFER, m_option.attachment, GL_TEXTURE_2D, m_texture, 0);
    }
    else if(m_option.texture_dim==GL_TEXTURE_3D){
        glBindTexture(GL_TEXTURE_3D, m_texture);
        glFramebufferTextureLayer(GL_FRAMEBUFFER, m_option.attachment, m_texture, 0, 0);
    }
    
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_renderbuffer);
    
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch(status) {
        case GL_FRAMEBUFFER_COMPLETE:
            m_current_layer = 0;
            break;
            
        case GL_FRAMEBUFFER_UNSUPPORTED:
            err_log("fbo unsupported");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            err_log("fbo GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
            break;
        default:
            err_log("Framebuffer Error: %x", status);
            break;
    }
    GPUCheckGlError("generateFrameBuffer", true, false);
}

void GPUFrameBuffer::setPixels(void *data){
    GPUContext::shareInstance()->makeCurrent();
    activeTexture(GL_TEXTURE0);
    glTexImage2D(GL_TEXTURE_2D, 0, m_option.color_format, m_width, m_height, 0, m_option.format, m_option.type, data);//装载贴图
    /** 3.0中才支持
    else if(m_option.texture_type == GL_TEXTURE_3D){
        glTexImage3D(GL_TEXTURE_3D, 0, m_option.color_format, m_width, m_height, m_depth, 0, m_option.format, m_option.type, data);//装载贴图
    }
    */
    // 默认过滤方式是GL_LINEAR_MIPMAP_LINEAR，需要生成贴图所有必要的mips
    if ( m_option.min_filter>=GL_NEAREST_MIPMAP_NEAREST ||  m_option.mag_filter>=GL_NEAREST_MIPMAP_NEAREST) {
        glGenerateMipmap( GL_TEXTURE_2D);
    }
}

uint8_t* GPUFrameBuffer::getPixels(uint8_t* data){
    // glReadPixels从显存中读取，必须先绑定到fbo
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_renderbuffer);
    glActiveTexture(GL_TEXTURE0);
    // 3D纹理时候
    glBindTexture(GL_TEXTURE_2D, m_texture);
    if (data==NULL) {
        if (m_rgba==NULL) {
            if (m_option.type == GL_FLOAT || m_option.type==GL_UNSIGNED_INT || m_option.type == GL_INT){
                m_rgba = (uint8_t*)malloc(m_width*m_height*4*4);
            }
            else{
                m_rgba = (uint8_t*)malloc(m_width*m_height*4);
            }
            assert(m_rgba!=NULL);
        }
        data = m_rgba;
    }
    glReadPixels(0, 0, m_width, m_height, m_option.format, m_option.type, data);
    unactive();
    return data;
}

void GPUFrameBuffer::activeTexture(GLuint texture_unit){
    glActiveTexture(texture_unit);
    if (m_option.texture_dim==GL_TEXTURE_2D) {
        glBindTexture(GL_TEXTURE_2D, m_texture);
    }
    else if(m_option.texture_dim==GL_TEXTURE_3D){
        glBindTexture(GL_TEXTURE_3D, m_texture);
    }
    // fbo不能设置filter，作为sample之前设置
    glTexParameteri(m_option.texture_dim, GL_TEXTURE_MIN_FILTER, m_option.min_filter);
    glTexParameteri(m_option.texture_dim, GL_TEXTURE_MAG_FILTER, m_option.mag_filter);
}

void GPUFrameBuffer::activeBuffer(int layer){
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    GPUCheckGlError("generateTexture", true, false);
    glBindRenderbuffer(GL_RENDERBUFFER, m_renderbuffer);
    GPUCheckGlError("generateTexture", true, false);
    if (m_option.texture_dim==GL_TEXTURE_2D) {
        // glFramebufferTexture2D(GL_FRAMEBUFFER, m_option.attachment, GL_TEXTURE_2D, m_texture, 0);
        glBindTexture(GL_TEXTURE_2D, m_texture);
    }
    if(m_option.texture_dim==GL_TEXTURE_3D && m_current_layer!=layer){
        glFramebufferTextureLayer(GL_FRAMEBUFFER, m_option.attachment, m_texture, 0, layer);
        glBindTexture(GL_TEXTURE_3D, m_texture);
        m_current_layer = layer;
    }
    
    GPUCheckGlError("generateTexture", true, false);
    glViewport(0, 0, (int)m_width, (int)m_height);
}

void GPUFrameBuffer::unactive(){
    glBindTexture(m_option.texture_dim, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GPUFrameBuffer::destroy(){
    GPUContext::shareInstance()->makeCurrent();

    if (m_framebuffer) {
        glDeleteFramebuffers(1, &m_framebuffer);
        glDeleteRenderbuffers(1, &m_renderbuffer);
        m_framebuffer = 0;
        m_renderbuffer = 0;
    }
    if (!m_outtexture) {
        glDeleteTextures(1, &m_texture);
        m_texture = 0;
    }
    if (m_rgba!=NULL) {
        free(m_rgba);
    }
}

bool GPUFrameBuffer::lock(){
    m_referencecount++;
    return true;
}

bool GPUFrameBuffer::unlock(){
    if (m_is_reference && m_referencecount<=0)
    {
        err_log("framebuffer[%d] unlock error! referencecount[%d]", m_texture, m_referencecount);
        return false;
    }
    
    if (m_is_reference) {
        m_referencecount--;
    }
    
    // err_log("texture:%d reference:%d", m_texture, m_referencecount);
    return true;
}

bool GPUFrameBuffer::idle(){
    return m_referencecount==0;
}

void GPUFrameBuffer::disableReference(){
    m_is_reference = false;
}
void GPUFrameBuffer::release(){
    m_is_reference = true;
    m_referencecount = 0;
}

gpu_frame_option_t GPUFrameBuffer::defaultFrameOption(){
    return g_frame_default_option;
}
gpu_frame_option_t* GPUFrameBuffer::nearestFrameOption(){
    return &g_frame_near_option;
}
gpu_frame_option_t* GPUFrameBuffer::uintFrameOption(){
    return &g_frame_uint_option;
}
gpu_frame_option_t* GPUFrameBuffer::floatFrameOption(){
    return &g_frame_float_option;
}
#pragma --mark "GPUBufferCache"
GPUBufferCache* GPUBufferCache::m_instance = NULL;

GPUBufferCache* GPUBufferCache::shareInstance(){
    if (m_instance==NULL)
    {
        m_instance = new GPUBufferCache();
    }
    return m_instance;
}

void GPUBufferCache::destroyInstance(){
    if (m_instance!=NULL)
    {
        m_instance->clear();
        delete m_instance;
        m_instance = NULL;
    }
}

GPUBufferCache::GPUBufferCache(){}

GPUFrameBuffer* GPUBufferCache::getFrameBuffer(int width, int height, bool only_texture){
    return getFrameBuffer(width, height, 0, &g_frame_default_option, only_texture);
}

GPUFrameBuffer* GPUBufferCache::getFrameBuffer(gpu_size_t size, bool only_texture){
    return getFrameBuffer(size.width, size.height, 0, &g_frame_default_option, only_texture);
}
GPUFrameBuffer* GPUBufferCache::getFrameBuffer(gpu_size_t size, gpu_frame_option_t* option, bool only_texture){
    return getFrameBuffer(size.width, size.height, 0, option, only_texture);
}
GPUFrameBuffer* GPUBufferCache::getFrameBuffer(int width, int height, gpu_frame_option_t* option, bool only_texture){
    return getFrameBuffer(width, height, 0, option, only_texture);
}
GPUFrameBuffer* GPUBufferCache::getFrameBuffer(int width, int height, int depth, gpu_frame_option_t* option, bool only_texture){
    GPUFrameBuffer* buffer=NULL;
    
    int i = 0;
    int n = (int)m_bufferlist.size();
    for (i = 0; i < n; ++i)
    {
        buffer = m_bufferlist[i];
        gpu_frame_option_t* op = &buffer->m_option;
        // 类型一致
        if (buffer->m_width==width && buffer->m_height==height && buffer->m_depth==depth// 长宽一致
            && (memcmp(op, option, sizeof(gpu_frame_option_t))==0)
//            && (op->texture_type==GL_TEXTURE_2D)     ///    格式参数相同
//            && (op->color_format==option->color_format)
//            && (op->type==option->type)
//            && (op->format == (int)option->format)
            && (only_texture==(buffer->m_framebuffer==0))   // 是否fbo的类型一致
            && buffer->idle())  // 是否空闲
        {
            return buffer;
        }
    }
    
    if (i >= m_bufferlist.size())
    {
        buffer = newFrameBuffer(width, height, depth, option, only_texture);
        debug_log("create: size[%d,%d] format[%x,%x,%x]", width, height, option->color_format, option->format, option->type);
        m_bufferlist.push_back(buffer);
    }
    
    return buffer;
}

GPUFrameBuffer* GPUBufferCache::newFrameBuffer(int width, int height, int depth, gpu_frame_option_t* option, bool only_texture){
    return new GPUFrameBuffer(width, height, depth, option, only_texture);
}

bool GPUBufferCache::setFrameBuffer(GPUFrameBuffer* buffer){
    m_bufferlist.push_back(buffer);
    return true;
}

void GPUBufferCache::clear(){
    GPUContext* context = GPUContext::shareInstance();
    context->makeCurrent();
    vector<GPUFrameBuffer*>::iterator it = m_bufferlist.begin();
    for (; it!=m_bufferlist.end(); it++) {
        delete *it;
    }
    m_bufferlist.clear();
}

void GPUBufferCache::purge(){
    vector<GPUFrameBuffer*>::iterator it = m_bufferlist.end();
    for (; it!=m_bufferlist.begin(); it--) {
        if ((*it)->idle()) {
            delete *it;
            m_bufferlist.erase(it);
        }
    }
}

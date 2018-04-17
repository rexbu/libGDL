/**
 * file :	GPUContext.cpp
 * author :	Rex
 * email : rex@labjk.com
 * create :	2016-04-27 19:30
 * func : 
 * history:
 */
#include "GPUContext.h"
#include "GPUProgram.h"
#include "GPUFrameBuffer.h"

GPUContext* GPUContext::m_instance = NULL;
bool GPUContext::m_has_context = true;

GPUContext* GPUContext::shareInstance(){
    if (m_instance==NULL) {
        m_instance = new GPUContext();
        if (m_has_context) {
            m_instance->createContext();
        }
    }
    return m_instance;
}

void GPUContext::destroyInstance(){
    if (m_instance!=NULL)
    {
        delete m_instance;
        m_instance = NULL;
    }
}

GPUContext::GPUContext(){
	m_current_program = NULL;
    pthread_mutex_init(&m_lock, NULL);

    m_has_context = true;
    m_gpu_context = NULL;
    
#ifdef __ANDROID__
    m_surfacewidth = 1080;
    m_surfaceheight = 1920;
#endif
}

void GPUContext::setActiveProgram(GPUProgram* program){
    makeCurrent();
	if(program != m_current_program){
		program->use();
        m_current_program = program;
    }
}

int GPUContext::maximumTextureSize(){
    GLint maxTextureSize = 0;
    
    makeCurrent();
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
    
    return maxTextureSize;
}

gpu_size_t GPUContext::sizeFitsTextureMaxSize(gpu_size_t inputSize){
    int maxTextureSize = maximumTextureSize();
    if ( (inputSize.width < maxTextureSize) && (inputSize.height < maxTextureSize) )
    {
        return inputSize;
    }
    
    gpu_size_t adjustedSize;
    if (inputSize.width > inputSize.height)
    {
        adjustedSize.width = maxTextureSize;
        adjustedSize.height = (maxTextureSize*1.0 / inputSize.width) * inputSize.height;
    }
    else
    {
        adjustedSize.height = maxTextureSize;
        adjustedSize.width = (maxTextureSize*1.0 / inputSize.height) * inputSize.width;
    }
    
    return adjustedSize;
}

int GPUContext::glContextLock(){
    return pthread_mutex_lock(&m_lock);
}
int GPUContext::glContextUnlock(){
    return pthread_mutex_unlock(&m_lock);
}

void GPUContext::printParams(){
    int param;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &param);
    info_log("max texture size: %d", param);
    glGetIntegerv(GL_DEPTH_BITS, &param);
    info_log("max depth count: %d", param);
    glGetIntegerv(GL_STENCIL_BITS, &param);
    info_log("max stencil count: %d", param);
    glGetIntegerv(GL_MAX_VARYING_VECTORS, &param);
    info_log("max varying count: %d", param);
    glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &param);
    info_log("max vertex texture_unit count: %d", param);
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &param);
    info_log("max fragment texture_unit count: %d", param);
}

int GPUContext::maxFragmentTextureCount(){
    static int param = -1;
    if (param==-1) {
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &param);
    }
    return param;
}

void GPUCheckGlError(const char* op, bool log, bool lock) {
    GPUContext* context;
    if (lock) {
        context = GPUContext::shareInstance();
        context->glContextLock();
    }
    
    const char* errors[] = {"GL_INVALID_ENUM", "GL_INVALID_VALUE", "GL_INVALID_OPERATION", "", "", "GL_OUT_OF_MEMORY", "GL_INVALID_FRAMEBUFFER_OPERATION"};
    for (GLint error = glGetError(); error!=GL_NO_ERROR; error = glGetError()) {
        if (log)
        {
            err_log("after %s() glError (%x:%s)", op, error, errors[error-GL_INVALID_ENUM]);
        }
    }
    if (lock) {
        context->glContextUnlock();
    }
}

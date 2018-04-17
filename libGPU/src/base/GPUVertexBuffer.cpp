/**
 * file :	GPUVertexBuffer.cpp
 * author :	Rex
 * email : rex@labjk.com
 * create :	2016-09-27 14:51
 * func : 
 * history:
 */

#include "GPUVertexBuffer.h"

#pragma --mark "GPUVertexBuffer"

GPUVertexBuffer::GPUVertexBuffer(uint32_t vertexCount, uint32_t dimension, bool cache){
    //vertexCount = 4;
    m_vertexcount = vertexCount;
    m_dimension = dimension;
    m_cache = cache;
    m_size = m_vertexcount*sizeof(GLfloat)*m_dimension;
    m_vertexbuffer = -1;
    
    if (m_cache) {
        glGenBuffers(1, &m_vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, m_size, NULL, GL_STREAM_DRAW);
    }
}

void GPUVertexBuffer::activeBuffer(GLuint attribute, const GLvoid *data, int offset)
{
    if (m_cache) {
        glEnableVertexAttribArray(attribute);
        glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);
        glBufferSubData(GL_ARRAY_BUFFER, offset, m_size, data);
        glVertexAttribPointer(attribute, m_dimension, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*m_dimension, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    else{
        glEnableVertexAttribArray(attribute);
        glVertexAttribPointer(attribute, m_dimension , GL_FLOAT, GL_FALSE, 0, data);
    }
}
void GPUVertexBuffer::activeBuffer(GLuint attribute){
    if (m_cache) {
        glEnableVertexAttribArray(attribute);
        glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);
        glVertexAttribPointer(attribute, m_dimension, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*m_dimension, 0);
    }
}

void GPUVertexBuffer::setBuffer(const GLvoid *data, int offset){
    if (m_cache) {
        glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);
        glBufferSubData(GL_ARRAY_BUFFER, offset, m_size, data);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void GPUVertexBuffer::disableBuffer(GLuint attribute){
    if (m_cache){
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDisableVertexAttribArray(attribute);
    }
    else{
        glDisableVertexAttribArray(attribute);
    }
}

GPUVertexBuffer::~GPUVertexBuffer(){
    if (m_cache){
        glDeleteBuffers(1, &m_vertexbuffer);
    }
}

#pragma --mark "GPUVertexBufferCache"

GPUVertexBufferCache* GPUVertexBufferCache::m_instance = NULL;
GPUVertexBufferCache* GPUVertexBufferCache::shareInstance(){
    if (m_instance==NULL) {
        m_instance = new GPUVertexBufferCache();
    }
    return m_instance;
}
void GPUVertexBufferCache::destroyInstance(){
    if (m_instance!=NULL) {
        delete m_instance;
        m_instance = NULL;
    }
}

GPUVertexBufferCache::GPUVertexBufferCache(){
}

GPUVertexBuffer* GPUVertexBufferCache::getVertexBuffer(uint32_t vertexCount, uint32_t dimension, bool cache){
    for (int i=0; i<m_bufferlist.size(); i++) {
        GPUVertexBuffer* buffer = m_bufferlist[i];
        if (buffer->idle() && buffer->getVertexCount()==vertexCount && buffer->getDimension()==dimension
            && buffer->isCache()==cache) {
            buffer->lock();
            return buffer;
        }
    }
    
    GPUVertexBuffer* buffer = newVertexBuffer(vertexCount, dimension, cache);
    buffer->lock();
    m_bufferlist.push_back(buffer);
    return buffer;
}

GPUVertexBuffer* GPUVertexBufferCache::newVertexBuffer(uint32_t vertexCount, uint32_t dimension, bool cache){
    GPUVertexBuffer* buffer = new GPUVertexBuffer(vertexCount, dimension, cache);
    return buffer;
}

void GPUVertexBufferCache::clear(){
    for (int i=0; i<m_bufferlist.size(); i++) {
        GPUVertexBuffer* buffer = m_bufferlist[i];
        delete buffer;
    }
    m_bufferlist.clear();
}

GPUVertexBufferCache::~GPUVertexBufferCache(){
    clear();
}

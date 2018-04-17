/**
 * file :	GPUVertexBuffer.h
 * author :	Rex
 * email : rex@labjk.com
 * create :	2016-09-27 14:51
 * func : 
 * history:
 */

#ifndef	__GPUVERTEXBUFFER_H_
#define	__GPUVERTEXBUFFER_H_

#include <vector>
#include "GPUContext.h"

using namespace std;

class BaseReference{
public:
    BaseReference(){
        m_referencecount = 0;
        m_is_reference = true;
    }
    
    bool lock(){
        if (m_is_reference) {
            m_referencecount++;
        }
        
        return true;
    }
    
    bool unLock(){
        if (m_is_reference && m_referencecount<=0)
        {
            err_log("unlock error! referencecount[%d]", m_referencecount);
            return false;
        }
        
        if (m_is_reference) {
            m_referencecount--;
        }
        
        return true;
    }
    
    bool idle(){
        return m_referencecount==0;
    }
    
    void disableReference(){
        m_referencecount = false;
    }
    
    bool release(){
        m_referencecount = 0;
        return true;
    }
    
protected:
    int     m_referencecount;
    bool    m_is_reference;
};

class GPUVertexBuffer: public BaseReference{
public:
    GPUVertexBuffer(uint32_t vertexCount = 4, uint32_t dimension = 2, bool cache = true);
    ~GPUVertexBuffer();
    
    GLuint  getVertexBuffer(){
        return m_vertexbuffer;
    }
    uint32_t getDimension(){
        return m_dimension;
    }
    uint32_t getVertexCount(){
        return m_vertexcount;
    }
    uint32_t getDataSize(){
        return m_size;
    }
    bool isCache(){
        return m_cache;
    }
    
    void activeBuffer(GLuint attribute, const GLvoid *data, int offset=0);
    void activeBuffer(GLuint attribute);
    void disableBuffer(GLuint attribute);
    void setBuffer(const GLvoid *data, int offset=0);
protected:
    uint32_t        m_vertexcount;
    uint32_t        m_dimension;
    uint32_t        m_size;
    GLuint          m_vertexbuffer;
    
    bool            m_cache;    // 是否使用顶点缓存
};

class GPUVertexBufferCache{
public:
    static GPUVertexBufferCache* shareInstance();
    static void destroyInstance();
    // static GPUVertexBuffer* defaultVertexBuffer();
    // static GPUVertexBuffer* defaultCoordinateBuffer();
    
    GPUVertexBufferCache();
    ~GPUVertexBufferCache();
    
    GPUVertexBuffer* getVertexBuffer(uint32_t vertexCount = 4 , uint32_t dimension = 2, bool cache = true);
    virtual GPUVertexBuffer* newVertexBuffer(uint32_t vertexCount = 4 , uint32_t dimension = 2, bool cache = true);
    void clear();
    /**
     * 设置是否使用顶点缓存。
     */
    static void setCache(bool cache){
        m_cache = cache;
    }
    
public:
    static GPUVertexBufferCache*    m_instance;
    static bool                     m_cache;
    
    vector<GPUVertexBuffer*>    m_bufferlist;
};
#endif

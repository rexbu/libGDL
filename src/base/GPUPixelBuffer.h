/**
 * file :	GPUPixelBuffer.h
 * author :	Rex
 * create :	2016-07-21 16:58
 * func : 
 * history:
 */

#ifndef	__GPUPIXELBUFFER_H_
#define	__GPUPIXELBUFFER_H_

#include "GPUContext.h"
#include "GPUFrameBuffer.h"

class GPUPixelBuffer{
public:
    /**
     * @size:   pbo大小，width*height, size*4为实际rgba容量
     * @sync:   为同步或者异步模式
     */
	GPUPixelBuffer(int size, bool sync = true, int elem_size=1);
    ~GPUPixelBuffer();
    void* read(GPUFrameBuffer* framebuffer);
    void unactive();
    
    inline void lock(){
        pthread_mutex_lock(&m_lock);
    }
    inline void unlock(){
        pthread_mutex_unlock(&m_lock);
    }
protected:
    GPUFrameBuffer* m_frame_buffer;
	GLuint		    m_buffer[2];
    GLuint          m_map_index;
	GLubyte*	    m_ptr;
    
    pthread_mutex_t m_lock;
	GLuint 		    m_pack_mode;
    uint32_t        m_size;
    bool            m_sync;     // 是否同步
    bool            m_first;    // 是否第一帧
};
#endif

/**
 * file :	GPUPixelBuffer.cpp
 * author :	Rex
 * create :	2016-07-21 16:58
 * func : 
 * history:
 */

#include "GPUPixelBuffer.h"

GPUPixelBuffer::GPUPixelBuffer(int size, bool sync, int elem_size){
    m_sync = sync;
    m_size = size;
    m_first = true;
    pthread_mutex_init(&m_lock, NULL);
    
	m_pack_mode = GL_PIXEL_PACK_BUFFER;
	glGenBuffers(2, m_buffer);
    m_map_index = 1;
    /*
     Stream-缓冲区的对象需要时常更新,但使用次数很少.
     Static-只需要一次指定缓冲区对象中的数据,但使用次数很多.
     Dynamic-数据不仅需要时常更新,使用次数也很多.
     */
    glBindBuffer(GL_PIXEL_PACK_BUFFER, m_buffer[0]);
    glBufferData(GL_PIXEL_PACK_BUFFER, size*4*elem_size, 0, GL_STATIC_READ);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, m_buffer[1]);
    glBufferData(GL_PIXEL_PACK_BUFFER, size*4*elem_size, 0, GL_STATIC_READ);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}

GPUPixelBuffer::~GPUPixelBuffer(){
    glUnmapBuffer(m_pack_mode);
    glDeleteBuffers(2, m_buffer);
    pthread_mutex_destroy(&m_lock);
}

void* GPUPixelBuffer::read(GPUFrameBuffer* framebuffer){
    m_frame_buffer = framebuffer;
    
    m_frame_buffer->activeBuffer();
    glBindBuffer(GL_PIXEL_PACK_BUFFER, m_buffer[1-m_map_index]);
    glReadPixels(0, 0, framebuffer->m_width, framebuffer->m_height, framebuffer->m_option.format, framebuffer->m_option.type, 0);

    if (m_sync) {
        glBindBuffer(GL_PIXEL_PACK_BUFFER, m_buffer[1-m_map_index]);
    }
    else{
        // 异步状态下的第一帧直接返回，在下一帧再映射当前帧的内存，这样交替进行提高效率
        if (m_first) {
            m_frame_buffer->unactive();
            m_map_index = 1 - m_map_index;
            glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
            m_first = false;
            return NULL;
        }
        
        glBindBuffer(GL_PIXEL_PACK_BUFFER, m_buffer[m_map_index]);
    }
    
    int elem_size = 1;
    if (framebuffer->m_option.type == GL_UNSIGNED_INT || framebuffer->m_option.type == GL_INT || framebuffer->m_option.type == GL_FLOAT) {
        elem_size = 4;
    }
    else if(framebuffer->m_option.type==GL_SHORT || framebuffer->m_option.type==GL_UNSIGNED_SHORT){
        elem_size = 2;
    }
    
    void* p = glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, framebuffer->m_width*framebuffer->m_height*4*elem_size, GL_MAP_READ_BIT);
    m_frame_buffer->unactive();
    m_map_index = 1 - m_map_index;
    unactive();
    
    return p;
}

void GPUPixelBuffer::unactive(){
    if (m_frame_buffer!=NULL) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    }
}
/*
void GPUPixelBuffer::upload(GLubyte* bytes, uint32_t format){
	unpack();
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, format, GL_UNSIGNED_BYTE, 0);
	switch(format){
	case GL_LUMINANCE:
		glBufferData(GL_PIXEL_UNPACK_BUFFER, m_width*m_height, bytes, GL_STREAM_DRAW);
		break;
	case GL_LUMINANCE_ALPHA:
		glBufferData(GL_PIXEL_UNPACK_BUFFER, m_width*m_height*2, bytes, GL_STREAM_DRAW);
		break;	
	default:
		glBufferData(GL_PIXEL_UNPACK_BUFFER, m_width*m_height*4, bytes, GL_STREAM_DRAW);
	}
}

void GPUPixelBuffer::unpack(){
	GPUContext::shareInstance()->makeCurrent();
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_buffer);
}

GLubyte* GPUPixelBuffer::download(){
	return m_ptr;
}

GLubyte* GPUPixelBuffer::download(GLuint texture, uint32_t width, uint32_t height){
	m_texture = texture;
	m_width = width;
	m_height = height;
	m_pack_mode = GPU_PACK;
	pack();
	return m_ptr;
}

void GPUPixelBuffer::pack(){
	GPUContext::shareInstance()->makeCurrent();
	glReadBuffer(GL_FRONT);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, m_buffer);
	glReadPixels(0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	// m_ptr = (GLubyte*)glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, m_width*m_height*4, GL_READ_WRITE);
}
*/

/**
 * file :	GPUFilter.h
 * author :	rex
 * email : rex@labjk.com
 * create :	2016-04-27 19:11
 * func :
 * history:
 */

#ifndef	__GPUINPUT_H_
#define	__GPUINPUT_H_

#include <iostream>
#include <vector>
#include "GL.h"
#include "GPUContext.h"

/**	SECTION - class definition
 */
class GPUFrameBuffer;
class GPUOutput;

class GPUInput {
public:
    GPUInput(int inputs = 1);
    ~GPUInput();
    
    virtual void setInputs(int inputs);

    virtual void newFrame();
    virtual void setInputFrameBuffer(GPUFrameBuffer *buffer, int location = 0);

	int addSource(GPUOutput *source);
	int removeSource(GPUOutput *source);
	virtual bool removeAllSources();
	int sourcesCount();

	inline virtual void setOutputRotation(gpu_rotation_t rotation) {
		m_rotation = rotation;
	}
    inline virtual gpu_rotation_t getOutputRotation(){
        return m_rotation;
    }
    inline virtual gpu_size_t getFrameSize(){
        gpu_size_t size = {m_frame_width, m_frame_height};
        return size;
    }
    
    inline virtual void disable(){
        m_disable = true;
    }
    inline virtual void enable(){
        m_disable = false;
    }

protected:
	virtual void setFrameSize(uint32_t width, uint32_t height);
	virtual bool ready(bool clearOnReady = true);		///	检查是否就绪
    void unlockInputFrameBuffers();
    
    GPUFrameBuffer*	m_firstbuffer;
    int             m_inputs;
	gpu_rotation_t  m_rotation;

    std::vector<GPUOutput*>         m_sources;
    std::vector<GPUFrameBuffer*>    m_input_buffers;
	int     m_ready;
	bool    m_disable;										///	禁用，不执行 newFrame（）

	// 帧尺寸
    uint32_t    m_frame_width;
    uint32_t    m_frame_height;
};


#endif

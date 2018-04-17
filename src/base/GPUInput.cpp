/**
 * file :	GPUFilter.h
 * author :	rex
 * email : rex@labjk.com
 * create :	2016-04-27 19:11
 * func :
 * history:
 */

#include "GPUInput.h"
#include "GPUOutput.h"
#include "GPUFrameBuffer.h"

GPUInput::GPUInput(int inputs)
{
    m_frame_width = 0;
    m_frame_height = 0;
    m_disable = false;
    m_ready = false;
    m_rotation = GPUNoRotation;
    m_firstbuffer = NULL;
	setInputs(inputs);
}

GPUInput::~GPUInput(){
    removeAllSources();
}

void GPUInput::setInputs(int inputs)
{
    if (inputs<=0) {
        return;
    }
    
	m_inputs = inputs;
    m_input_buffers.resize(m_inputs);
    memset(&m_input_buffers[0], 0, sizeof(GPUFrameBuffer*)*m_inputs);
	
    m_ready = 0;
	// removeAllSources();
}

int GPUInput::addSource(GPUOutput *source)
{
    m_sources.push_back(source);
    return (int)m_sources.size();
}

int GPUInput::removeSource(GPUOutput *source)
{
    vector<GPUOutput*>::iterator iter = std::find(m_sources.begin(), m_sources.end(), source);
    if (iter!=m_sources.end()) {
        m_sources.erase(iter);
    }
    
    return (int)m_sources.size();
}

int GPUInput::sourcesCount()
{
	return (int)m_sources.size();
}

bool GPUInput::removeAllSources()
{
    for (int i=0; i<sourcesCount(); i++) {
        GPUOutput* source = m_sources[i];
        if (source!=NULL) {
            source->removeTarget(this);
        }
    }
    
    m_sources.clear();
	return true;
}

void GPUInput::newFrame()
{
	if(ready(true))
		unlockInputFrameBuffers();
}

void GPUInput::setInputFrameBuffer(GPUFrameBuffer* buffer, int location)
{
	if(location >= m_inputs || location<0) {
		err_log("GPUInput::setInputFrameBuffer(%d) overflow!", location);
		return;
	}
    if (buffer==NULL) {
        err_log("setInputFrameBuffer NULL!");
        return;
    }
    
    m_input_buffers[location] = buffer;
	buffer->lock();

    if(location == 0){
        m_firstbuffer = buffer;
        // 存储帧大小
        if (m_frame_width!=buffer->m_width || m_frame_height!=buffer->m_height)
        {
            setFrameSize(buffer->m_width, buffer->m_height);
        }
    }
}

void GPUInput::setFrameSize(uint32_t width, uint32_t height){
    m_frame_width = width;
    m_frame_height = height;
}

void GPUInput::unlockInputFrameBuffers()
{
    for (int i=0; i<m_inputs; i++) {
        if (m_input_buffers[i]==NULL) {
            err_log("Visionin: input framebuffer[%d] is NULL!", i);
            continue;
        }
        m_input_buffers[i]->unlock();
    }
    
    memset(&m_input_buffers[0], 0, sizeof(GPUFrameBuffer*)*m_inputs);
    m_ready = false;
}

bool GPUInput::ready(bool clearOnReady)
{
    // 检查就绪状态
    for (int i=0; i<m_inputs; i++) {
        if (m_input_buffers[i] == NULL) {
            return false;
        }
    }
    
    m_ready = true;
    if (m_ready) {
        if (clearOnReady)
        {
            m_ready = false;
        }
        return true;
    }
    
    return false;
}

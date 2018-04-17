/**
 * file :	GPUOutput.cpp
 * author :	Rex
 * email : rex@labjk.com
 * create :	2016-04-27 19:12
 * func :
 * history:
 */

#include "bs.h"
#include "GPUOutput.h"
#include "GPUFrameBuffer.h"

/**	SECTION - class definition
 */
GPUOutput::GPUOutput()
{
	m_complete = NULL;
	m_para = NULL;
	m_out_width = 0;
	m_out_height = 0;
	m_outbuffer = NULL;
}

void GPUOutput::reseto()
{
	removeAllTargets();
	m_complete = NULL;
	m_para = NULL;
}

void GPUOutput::informTargets()
{
	int i, n = targetsCount();
    // 注意，此处要先全部lock，否则1->2->3 情况下，当2执行完，1的outbuffer会被释放
    //                          ->4
	for(i = 0; i < n; i ++) {
		GPUInput* input = m_targets[i];
		input->setInputFrameBuffer(m_outbuffer, m_location[i]);
	}
    for(i = 0; i < n; i ++) {
        GPUInput* input = m_targets[i];
        input->newFrame();
    }
}

void GPUOutput::setCompleteCallback(GPUCompleteCallback_f complete, void *para)
{
	m_complete = complete;
	m_para = para;
}

int GPUOutput::targetsCount()
{
	return m_targets.size();
}

GPUInput* GPUOutput::getTarget(int i, int &location)
{
	if(i >= m_targets.size())
		return NULL;
	if(i < 0)
		if((i = m_targets.size()-1) < 0)
			return NULL;
	location = m_location[i];
	return m_targets[i];
}

int GPUOutput::addTarget(GPUInput *target, int location)
{
	if(!target) {
		err_log("addTarget NULL!");
		return -1;
	}
    
    int target_count = (int)m_targets.size();
    for (int i=0; i < target_count; i++) {
        // 如果查找到target重置location
        if (m_targets[i] == target) {
            m_location[i] = location;
            return location;
        }
    }
    
	target->addSource(this);
    m_targets.push_back(target);
    m_location.push_back(location);
	return location;
}

bool GPUOutput::removeTarget(GPUInput *target, int *location)
{
	int i = (int)m_targets.size();
	if(i <= 0) {
		err_log("GPUOutput::removeTarget() from empty target list!");
		return false;
	}
	
    vector<GPUInput*>::iterator targetIter = m_targets.begin();
    vector<int>::iterator locationIter = m_location.begin();
    for (; targetIter!=m_targets.end(); targetIter++,locationIter++) {
        if (*targetIter == target) {
            m_targets.erase(targetIter);
            m_location.erase(locationIter);
            return true;
        }
    }
    
	return false;
}

bool GPUOutput::removeAllTargets()
{
    for (int i=0; i<m_targets.size(); i++) {
        m_targets[i]->removeSource(this);
    }
    
    m_targets.clear();
    m_location.clear();
	return true;
}

void GPUOutput::setOutputSize(uint32_t width, uint32_t height){
    m_out_width = width;
    m_out_height = height;
}

gpu_size_t GPUOutput::getOutputSize(){
    gpu_size_t size = {m_out_width, m_out_height};
    return size;
};

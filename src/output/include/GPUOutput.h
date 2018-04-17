/**
 * file :	GPUOutput.h
 * author :	Rex
 * email : rex@labjk.com
 * create :	2016-04-27 19:11
 * func : 
 * history:
 */

#ifndef	__GPUOUTPUT_H_
#define	__GPUOUTPUT_H_

#include <iostream>
#include <vector>
#include "GPUInput.h"

using namespace std;
class GPUOutput;
typedef void (*GPUCompleteCallback_f)(GPUOutput *output, void *para);

class GPUOutput {
public:
	GPUOutput();
	~GPUOutput() {
		reseto();
	}
	
	void reseto();

	///	连接/删除（所有）下级连接
	virtual int addTarget(GPUInput *target, int location = 0);
	virtual bool removeTarget(GPUInput *target, int *location = NULL);
	virtual bool removeAllTargets();
	virtual void informTargets();
	
	int targetsCount();
	GPUInput* getTarget(int i, int &location);

	///////////////////////////////////////////////////////
	///	设置回调函数与回调参数
	void setCompleteCallback(GPUCompleteCallback_f complete, void* para);

	virtual void setOutputSize(uint32_t width, uint32_t height);
    gpu_size_t getOutputSize();

    GPUFrameBuffer*			m_outbuffer;
protected:
	std::vector<GPUInput*>	m_targets;
	std::vector<int> 		m_location;

	GPUCompleteCallback_f 	m_complete;
	void*					m_para;

	// 输出尺寸
	uint32_t    m_out_width;
	uint32_t    m_out_height;
};

#endif

/**
 * file :	GPUPipeline.cpp
 * author :	Rex
 * create :	2017-09-25 22:27
 * func : 
 * history:
 */

#include "GPUPipeline.h"

GPUPipeline::GPUPipeline(const char* names[], int size){
	// 必须至少有一个输入一个输出
	if (size < 2)
	{
		return;
	}

	m_input = input(names[0]);
	m_output = output(names[size-1]);

	GPUOutput* prev = m_input;
	for (int i = 1; i < size-1; ++i)
	{
		GPUFilter* f = filter(names[i]);
		prev->addTarget(f);
		m_filters[string(names[i])]	= f;
		prev = f;
	}

	prev->addTarget(m_output);
}

GPUPipeline::~GPUPipeline(){
	if (m_input!=NULL)
	{
		delete m_input;
	}
	if (m_output!=NULL)
	{
		delete m_output;
	}

	map<string, GPUFilter*>::iterator iter = m_filters.begin();
	for (; iter!=m_filters.end(); iter++)
	{
		delete iter->second;
	}
	m_filters.clear();
}

bool GPUPipeline::loadImage(const char* path){
	return ((GPUPicture*)m_input)->load(path);
}

bool GPUPipeline::loadImage(uint8_t* bytes, uint32_t size){
	return ((GPUPicture*)m_input)->load(bytes, size);
}

void GPUPipeline::processImage(){
	((GPUPicture*)m_input)->processImage();
}

uint8_t*  GPUPipeline::getBytes(uint8_t* buffer, uint32_t size){
	return ((GPURawOutput*)m_output)->getBuffer(buffer, size);
}

uint32_t GPUPipeline::rawOutSize(){
	return ((GPURawOutput*)m_output)->getSize();
}
GPUOutput* GPUPipeline::input(const char* name){
	if (strcmp("GPUPicture", name) == 0)
	{
		return new GPUPicture();
	}
	else if(strcmp("GPUCamera", name)==0){

	}
	else if (strcmp("GPURawInput", name)==0)
	{
		return new GPURawInput();
	}

	return NULL;
}

GPUFilter* GPUPipeline::filter(const char* name){
	if (strcmp("GPUMosaicFilter", name)==0)
	{
		return new GPUPixellatePositionFilter();
	}

	return NULL;
}

GPUInput* GPUPipeline::output(const char* name){
	if (strcmp("GPUView", name)==0)
	{
		return new GPUView(720, 1080);
	}
	else if (strcmp("GPURawOutput", name)==0)
	{
		return new GPURawOutput();
	}
}

void GPUPipeline::setMosaicBlockSize(float size){
	map<string, GPUFilter*>::iterator iter = m_filters.find(string("GPUMosaicFilter"));
	if (iter!=m_filters.end())
	{
		GPUPixellatePositionFilter* filter = (GPUPixellatePositionFilter*)iter->second;
		filter->setExtraParameter(size);	
	}
}

void GPUPipeline::setMosaicCircle(gpu_point_t center, float radius){
	map<string, GPUFilter*>::iterator iter = m_filters.find(string("GPUMosaicFilter"));
	if (iter!=m_filters.end())
	{
		GPUPixellatePositionFilter* filter = (GPUPixellatePositionFilter*)iter->second;
		filter->adjustPixellate(center, radius);	
	}
}
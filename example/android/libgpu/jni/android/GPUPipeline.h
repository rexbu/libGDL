/**
 * file :	GPUPipeline.h
 * author :	Rex
 * create :	2017-09-25 22:26
 * func : 
 * history:
 */

#ifndef	__GPUPIPELINE_H_
#define	__GPUPIPELINE_H_

#include <iostream>
#include <map>
#include "GPU.h"

using namespace std;

class GPUPipeline{
public:
	GPUPipeline(const char* names[], int size);
	~GPUPipeline();

	bool loadImage(const char* path);
	bool loadImage(uint8_t* bytes, uint32_t size);
	void processImage();
	uint8_t* getBytes(uint8_t* buffer=NULL, uint32_t size=0);
	uint32_t rawOutSize();
	/**
	 * 设置马赛克方块
	 * @param size 方块大小
	 */
	void setMosaicBlockSize(float size);
	/**
	 * 设置马赛克圆圈
	 * @param center 圆心
	 * @param radius 半径
	 */
	void setMosaicCircle(gpu_point_t center, float radius);

	static GPUOutput* input(const char* name);
	static GPUFilter* filter(const char* name);
	static GPUInput* output(const char* name);

protected:
	GPUOutput*				m_input;
	GPUInput*				m_output;
	map<string, GPUFilter*> m_filters;
};

#endif

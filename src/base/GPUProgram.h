/**
 * file :	GPUProgram.h
 * author :	Rex
 * email : rex@labjk.com
 * create :	2016-05-09 22:30
 * func : 
 * history:
 */

#ifndef	__GPUPROGRAM_H_
#define	__GPUPROGRAM_H_

#include "GL.h"
#include "GPUContext.h"

class GPUProgram{
public:
	GPUProgram();
	GPUProgram(const char* vertex, const char* fragment, const char* filter = NULL);
	~GPUProgram() {
		clearProgram();
	}

	GLint attributeIndex(const char* name);
	GLint uniformIndex(const char* name);
    void setUniformsIndex(const char* name, GLint* index, uint32_t size);

	void link();
	void use();

	GLuint getProgram(){
		return m_shader_program;
	}

	int setProgram(const char *fragment, const char *vertex = NULL);

	void setInteger(const char *name, int val);
    void setIntegerv(const char *name, int* val, int num);
    void setUIntegerv(const char *name, uint32_t* val, int num);
	void setFloat(const char* name, GLfloat val);
	void setFloat(const char* name, GLfloat* val, int num);
    void setFloatv(const char* name, GLfloat* val, int num);
	void setMatrix(const char* name, GLfloat* val, int num);
	void enableAttribArray(const char* name);

	const char* m_vertex_str;
	const char* m_fragment_str;
	const char* m_filter;
protected:
	void compile(const char* vertex, const char* fragment);
	GLuint loadShader(GLenum shaderType, const char* pSource);

protected:
	GLuint m_shader_program;
	GLuint m_vertex;
	GLuint m_fragment;
	bool m_shader_used;

	void clearProgram();
};

#endif

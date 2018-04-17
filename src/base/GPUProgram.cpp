/**
 * file :	GPUProgram.cpp
 * author :	Rex
 * email : rex@labjk.com
 * create :	2016-05-09 22:30
 * func : 
 * history:
 */

#include "GPUProgram.h"

GPUProgram::GPUProgram(){
    m_vertex_str = m_fragment_str = NULL;
    m_vertex = m_fragment = m_shader_program = 0;
}

GPUProgram::GPUProgram(const char* vertex, const char* fragment, const char* filter){
    m_vertex_str = m_fragment_str = NULL;
    m_vertex = m_fragment = m_shader_program = 0;
    m_filter = filter;
	setProgram(fragment, vertex);
}

void GPUProgram::clearProgram(){
    GPUContext::shareInstance()->makeCurrent();
	
    m_shader_used = false;
	m_vertex_str = m_fragment_str = NULL;
	
    if(m_vertex)
		glDeleteShader(m_vertex);
	if(m_fragment)
		glDeleteShader(m_fragment);
	m_vertex = m_fragment = 0;
    
    if(m_shader_program){
        m_shader_program = 0;
        glDeleteProgram(m_shader_program);
    }
}

int GPUProgram::setProgram(const char *fragment, const char *vertex){
	clearProgram();
	if(vertex)
		m_vertex_str = vertex;
	if(fragment)
		m_fragment_str = fragment;
	compile(m_vertex_str, m_fragment_str);
	m_shader_used = false;
	return m_shader_program;
}

void GPUProgram::compile(const char* vertex, const char* fragment){
    GPUContext* context =  GPUContext::shareInstance();
    context->glContextLock();
    context->makeCurrent();
    
    m_vertex = loadShader(GL_VERTEX_SHADER, vertex);
    if (!m_vertex) {
        err_log("Visionin Error: Link Vertex Shader Error![%d]", m_vertex);
        context->glContextUnlock();
        return;
    }
    m_fragment = loadShader(GL_FRAGMENT_SHADER, fragment);
    if (!m_fragment) {
        err_log("Visionin Error: Link Framgment Shader Error![%d]", m_fragment);
        context->glContextUnlock();
        return;
    }

    m_shader_program = glCreateProgram();
    if (m_shader_program) {
        glAttachShader(m_shader_program, m_vertex);
        glAttachShader(m_shader_program, m_fragment);
        glLinkProgram(m_shader_program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(m_shader_program, GL_LINK_STATUS, &linkStatus);
        if(linkStatus != GL_TRUE){
            err_log("Visionin Error: Link program Error[%d]!", linkStatus);
            GLint bufLength = 0;
            glGetProgramiv(m_shader_program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                char* buf = (char*) malloc(bufLength);
                if (buf) {
                    glGetProgramInfoLog(m_shader_program, bufLength, NULL, buf);
                    err_log("Visionin Error: Could not link program:\n%s\n", buf);
                    free(buf);
                }
            }
            glDeleteProgram(m_shader_program);
            m_shader_program = 0;
        }
    }
    else{
        err_log("Visionin Error: Link Program Error! Program is 0");
    }
    context->glContextUnlock();
}

void GPUProgram::link(){
	glLinkProgram(m_shader_program);
    if(m_vertex)
        glDeleteShader(m_vertex);
    if(m_fragment)
        glDeleteShader(m_fragment);
    m_vertex = m_fragment = 0;
}

void GPUProgram::use(){
	glUseProgram(m_shader_program);
}

GLuint GPUProgram::loadShader(GLenum shaderType, const char* pSource){
	GLuint shader = glCreateShader(shaderType);
    if (shader) {
        glShaderSource(shader, 1, &pSource, NULL);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen) {
                char* buf = (char*) malloc(infoLen);
                memset(buf, 0, infoLen);
                if (buf) {
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    err_log("Could not compile shader %d:\n%s\n",shaderType, buf);
                    free(buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    }
    else{
        err_log("glCreateShader Error[type:%d]", shaderType);
    }
    return shader;
}

GLint GPUProgram::attributeIndex(const char* name){
    use();
	GLint index = glGetAttribLocation(m_shader_program, name);
	if(index < 0){
		err_log("filter[%s] attributeIndex(%s) not found!", m_filter, name);
        //err_log("vertex: %s", m_vertex_str);
        GPUCheckGlError("glGetAttribLocation", true, false);
	}
    GPUCheckGlError("glGetAttribLocation", true, false);
	return index;
}

GLint GPUProgram::uniformIndex(const char* name){
    use();
	GLint index = glGetUniformLocation(m_shader_program, name);
    if(index < 0){
		err_log("filter[%s] uniformIndex(%s) not found!", m_filter, name);
        GPUCheckGlError("glGetUniformLocation", true, false);
    }
	return index;
}

void GPUProgram::setUniformsIndex(const char* name, GLint* index, uint32_t size){
    GLint location = uniformIndex(name);
    for (int i=0; i<size; i++) {
        index[i] = location+i;
    }
    if (location>=0) {
        glUniform1iv(location, size, index);
    }
    GPUCheckGlError("setUniformsIndex", true, false);
}

void GPUProgram::setInteger(const char *name, int val){
    GPUContext* context = GPUContext::shareInstance();
    context->glContextLock();
    context->setActiveProgram(this);
	GLint index = uniformIndex(name);
	if(index >= 0)
		glUniform1i(index, val);

	context->glContextUnlock();
}

void GPUProgram::setIntegerv(const char *name, int* val, int num){
    GPUContext* context = GPUContext::shareInstance();
    context->glContextLock();
    context->setActiveProgram(this);
    GLint index = uniformIndex(name);
    glUniform1iv(index, num, val);
    
    context->glContextUnlock();
}

void GPUProgram::setUIntegerv(const char *name, uint32_t* val, int num){
    GPUContext* context = GPUContext::shareInstance();
    context->glContextLock();
    context->setActiveProgram(this);
    GLint index = uniformIndex(name);
    glUniform1uiv(index, num, val);
    
    context->glContextUnlock();
}

void GPUProgram::setFloat(const char* name, GLfloat val){
	setFloat(name, &val, 1);
}

void GPUProgram::setFloat(const char* name, GLfloat* val, int num){
	GPUContext* context = GPUContext::shareInstance();
    context->glContextLock();
    context->setActiveProgram(this);

	GLint index = uniformIndex(name);
	if(index >= 0) {
		switch(num){
		case 1:	glUniform1f(index, *val);
				break;
        case 2:	glUniform2fv(index, 1, val);
				break;
        case 3:	glUniform3fv(index, 1, val);
				break;
        case 4: glUniform4fv(index, 1, val);
				break;
		}
	}
	
    context->glContextUnlock();
}

void GPUProgram::setFloatv(const char* name, GLfloat* val, int num){
	GPUContext* context = GPUContext::shareInstance();
    context->glContextLock();
    context->setActiveProgram(this);

	GLint index = uniformIndex(name);
	if(index >= 0)
		glUniform1fv(index, num, val);
	
    context->glContextUnlock();
}

void GPUProgram::setMatrix(const char* name, GLfloat* val, int num){
	GPUContext* context = GPUContext::shareInstance();
    context->glContextLock();
    context->setActiveProgram(this);

	GLint index = uniformIndex(name);
	if(index >= 0) {
		switch(num){
        case 2:	glUniformMatrix2fv(index, 1, GL_FALSE, val);
				break;
        case 3:	glUniformMatrix3fv(index, 1, GL_FALSE, val);
				break;
        case 4: glUniformMatrix4fv(index, 1, GL_FALSE, val);
				break;
		}
	}
	
    context->glContextUnlock();
}

void GPUProgram::enableAttribArray(const char* name){
	GLint index = attributeIndex(name);
    if(index >= 0){
		glEnableVertexAttribArray(index);
    }
    else{
        err_log("enableAttribArray %s error!", name);
    }
}

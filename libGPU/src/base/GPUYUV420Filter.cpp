/**
 * file :	GPUYUV420Filter.cpp
 * author :	Rex
 * email : rex@labjk.com
 * create :	2016-07-22 18:37
 * func : 
 * history:
 */

#include "GPUYUV420Filter.h"

#pragma --mark "To I420"
static const char* g_toyuv420_fragment_shader = SHADER_STRING(
	varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;

    uniform mediump float frameHStep;	// 1.0/(frameHeight-1)
    uniform mediump float wStep;	// 1.0/(420width-1)
    uniform mediump float hStep;	// 1.0/(420height-1)

    const mediump float yh = 2.0/3.0;
    const mediump float uh = 5.0/6.0;
    const mediump float ystep = 1.0/4.0;
    const mediump float ustep = 1.0/8.0;
    void main()
    {
    	// 第几行
    	mediump float h = textureCoordinate.y/hStep;
    	if (textureCoordinate.y<yh)
    	{
    		mediump float x = 4.0*(textureCoordinate.x-ystep*floor(textureCoordinate.x/ystep));
    		mediump float y = h*frameHStep*4.0 + floor(textureCoordinate.x/ystep)*frameHStep;
    		mediump vec4 p0 = texture2D(inputImageTexture, vec2(x, y));
    		mediump vec4 p1 = texture2D(inputImageTexture, vec2(x+wStep, y));
    		mediump vec4 p2 = texture2D(inputImageTexture, vec2(x+2.0*wStep, y));
    		mediump vec4 p3 = texture2D(inputImageTexture, vec2(x+3.0*wStep, y));
    		gl_FragColor = vec4(p0.r, p1.r, p2.r, p3.r);
    	}
    	else if (textureCoordinate.y<uh)
    	{
    		mediump float x = 8.0*(textureCoordinate.x-ustep*floor(textureCoordinate.x/ustep));
    		mediump float y = (h-yh/hStep)*frameHStep*16.0 + 2.0*floor(textureCoordinate.x/ustep)*frameHStep;
    		mediump vec4 p0 = texture2D(inputImageTexture, vec2(x, y));
    		mediump vec4 p1 = texture2D(inputImageTexture, vec2(x+2.0*wStep, y));
    		mediump vec4 p2 = texture2D(inputImageTexture, vec2(x+4.0*wStep, y));
    		mediump vec4 p3 = texture2D(inputImageTexture, vec2(x+6.0*wStep, y));
    		gl_FragColor = vec4(p0.g, p1.g, p2.g, p3.g);
    	}
    	else{
    		mediump float x = 8.0*(textureCoordinate.x-ustep*floor(textureCoordinate.x/ustep));
    		mediump float y = (h-uh/hStep)*frameHStep*16.0 + 2.0*floor(textureCoordinate.x/ustep)*frameHStep;
    		mediump vec4 p0 = texture2D(inputImageTexture, vec2(x, y));
    		mediump vec4 p1 = texture2D(inputImageTexture, vec2(x+2.0*wStep, y));
    		mediump vec4 p2 = texture2D(inputImageTexture, vec2(x+4.0*wStep, y));
    		mediump vec4 p3 = texture2D(inputImageTexture, vec2(x+6.0*wStep, y));
    		gl_FragColor = vec4(p0.b, p1.b, p2.b, p3.b);
    	}
    }
);

static const char* g_toyuv420_narrow_fragment_shader = SHADER_STRING(
    varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;

    uniform mediump float frameHStep;   // 1.0/(frameHeight-1)
    uniform mediump float frameWStep;
    uniform mediump float wStep;    // 1.0/(420width-1)
    uniform mediump float hStep;    // 1.0/(420height-1)

    const mediump float yh = 2.0/3.0;
    const mediump float uh = 5.0/6.0;
    const mediump float ystep = 1.0/4.0;
    const mediump float ustep = 1.0/8.0;
    void main()
    {
        if (textureCoordinate.y<yh)
        {
            // 在原纹理中第几行
            mediump float oy = textureCoordinate.y*1.5;
            mediump vec4 p0 = texture2D(inputImageTexture, vec2(textureCoordinate.x, oy));
            mediump vec4 p1 = texture2D(inputImageTexture, vec2(textureCoordinate.x+frameWStep, oy));
            mediump vec4 p2 = texture2D(inputImageTexture, vec2(textureCoordinate.x+2.0*frameWStep, oy));
            mediump vec4 p3 = texture2D(inputImageTexture, vec2(textureCoordinate.x+3.0*frameWStep, oy));
            gl_FragColor = vec4(p0.r, p1.r, p2.r, p3.r);
        }
        else if (textureCoordinate.y<uh)
        {
            mediump float x = 2.0*(textureCoordinate.x-0.5*floor(textureCoordinate.x*2.0));
            mediump float y = 4.0*frameHStep*(textureCoordinate.y/hStep-yh/hStep) + 2.0*frameHStep*floor(textureCoordinate.x*2.0);
            mediump vec4 p0 = texture2D(inputImageTexture, vec2(x, y));
            mediump vec4 p1 = texture2D(inputImageTexture, vec2(x+2.0*frameWStep, y));
            mediump vec4 p2 = texture2D(inputImageTexture, vec2(x+4.0*frameWStep, y));
            mediump vec4 p3 = texture2D(inputImageTexture, vec2(x+6.0*frameWStep, y));
            gl_FragColor = vec4(p0.g, p1.g, p2.g, p3.g);
        }
        else{
            mediump float x = 2.0*(textureCoordinate.x-0.5*floor(textureCoordinate.x*2.0));
            mediump float y = 4.0*frameHStep*(textureCoordinate.y/hStep-uh/hStep) + 2.0*frameHStep*floor(textureCoordinate.x*2.0);
            mediump vec4 p0 = texture2D(inputImageTexture, vec2(x, y));
            mediump vec4 p1 = texture2D(inputImageTexture, vec2(x+2.0*frameWStep, y));
            mediump vec4 p2 = texture2D(inputImageTexture, vec2(x+4.0*frameWStep, y));
            mediump vec4 p3 = texture2D(inputImageTexture, vec2(x+6.0*frameWStep, y));
            gl_FragColor = vec4(p0.b, p1.b, p2.b, p3.b);
        }
    }
);

GPUToYUV420Filter::GPUToYUV420Filter():
GPUFilter(g_toyuv420_narrow_fragment_shader){
    m_filter_name = "YUV420Filter";
}

GPUToYUV420Filter::GPUToYUV420Filter(const char* fragment):GPUFilter(fragment){
}

gpu_size_t GPUToYUV420Filter::sizeOfFBO(){
	if (m_input_buffers[0]->m_width%4!=0)
	{
		err_log("Visionin Error: frame width[%d] is not multiple of 4. YUV420 data may be wrong!", m_input_buffers[0]->m_width);
	}
	if (m_input_buffers[0]->m_height%8 != 0)
	{
		//err_log("Visionin Error: frame height[%d] is not multiple of 8. YUV420 data may be wrong!", m_input_buffers[0]->m_height);
	}
	return GPUFilter::sizeOfFBO();
}

void GPUToYUV420Filter::setFrameSize(uint32_t width, uint32_t height){
    GPUFilter::setFrameSize(width, height);
    setOutputSize(m_frame_width/4, m_frame_height*3/2);
    
    setFloat("frameHStep", 1.0/(m_frame_height-1));
    setFloat("frameWStep", 1.0/(m_frame_width-1));
    //setFloat("wStep", 1.0/(m_frame_width/4-1));
    setFloat("hStep", 1.0/(m_frame_height*3/2-1));
}

#pragma --mark "To NV21"
static const char* g_tonv21_fragment_shader = SHADER_STRING(
	varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;

    uniform mediump float frameHStep;	// 1.0/(frameHeight-1)
    uniform mediump float wStep;	// 1.0/(420width-1)
    uniform mediump float hStep;	// 1.0/(420height-1)

    const mediump float yh = 2.0/3.0;
    const mediump float uh = 5.0/6.0;
    const mediump float ystep = 1.0/4.0;
    const mediump float ustep = 1.0/4.0;
    void main()
    {
    	// 第几行
    	mediump float h = textureCoordinate.y/hStep;
    	if (textureCoordinate.y<yh)
    	{
    		mediump float x = 4.0*(textureCoordinate.x-ystep*floor(textureCoordinate.x/ystep));
    		mediump float y = h*frameHStep*4.0 + floor(textureCoordinate.x/ystep)*frameHStep;
    		mediump vec4 p0 = texture2D(inputImageTexture, vec2(x, y));
    		mediump vec4 p1 = texture2D(inputImageTexture, vec2(x+wStep, y));
    		mediump vec4 p2 = texture2D(inputImageTexture, vec2(x+2.0*wStep, y));
    		mediump vec4 p3 = texture2D(inputImageTexture, vec2(x+3.0*wStep, y));
    		gl_FragColor = vec4(p0.r, p1.r, p2.r, p3.r);
    	}
    	else
        {
    		mediump float x = 4.0*(textureCoordinate.x-ustep*floor(textureCoordinate.x/ustep));
    		mediump float y = (h-yh/hStep)*frameHStep*8.0 + 2.0*floor(textureCoordinate.x/ustep)*frameHStep;
    		mediump vec4 p0 = texture2D(inputImageTexture, vec2(x, y));
    		mediump vec4 p1 = texture2D(inputImageTexture, vec2(x+2.0*wStep, y));
    		gl_FragColor = vec4(p0.b, p0.g, p1.b, p1.g);
    	}
    }
);

GPUToNV21Filter::GPUToNV21Filter():GPUToYUV420Filter(g_tonv21_fragment_shader){
    m_filter_name = "NV21Filter";
}
void GPUToNV21Filter::setFrameSize(uint32_t width, uint32_t height){
    GPUFilter::setFrameSize(width, height);
    setOutputSize(m_frame_width, m_frame_height*3/8);
    
    setFloat("frameHStep", 1.0/(m_frame_height-1));
    setFloat("frameWStep", 1.0/(m_frame_width-1));
    //setFloat("wStep", 1.0/(m_frame_width/4-1));
    setFloat("hStep", 1.0/(m_frame_height*3/8-1));
}

#pragma --mark "To NV12"
static const char* g_tonv12_fragment_shader = SHADER_STRING(
	varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;

    uniform mediump float frameHStep;	// 1.0/(frameHeight-1)
    uniform mediump float wStep;	// 1.0/(420width-1)
    uniform mediump float hStep;	// 1.0/(420height-1)

    const mediump float yh = 2.0/3.0;
    const mediump float uh = 5.0/6.0;
    const mediump float ystep = 1.0/4.0;
    const mediump float ustep = 1.0/4.0;
    void main()
    {
    	// 第几行
    	mediump float h = textureCoordinate.y/hStep;
    	if (textureCoordinate.y<yh)
    	{
    		mediump float x = 4.0*(textureCoordinate.x-ystep*floor(textureCoordinate.x/ystep));
    		mediump float y = h*frameHStep*4.0 + floor(textureCoordinate.x/ystep)*frameHStep;
    		mediump vec4 p0 = texture2D(inputImageTexture, vec2(x, y));
    		mediump vec4 p1 = texture2D(inputImageTexture, vec2(x+wStep, y));
    		mediump vec4 p2 = texture2D(inputImageTexture, vec2(x+2.0*wStep, y));
    		mediump vec4 p3 = texture2D(inputImageTexture, vec2(x+3.0*wStep, y));
    		gl_FragColor = vec4(p0.r, p1.r, p2.r, p3.r);
    	}
    	else
        {
    		mediump float x = 4.0*(textureCoordinate.x-ustep*floor(textureCoordinate.x/ustep));
    		mediump float y = (h-yh/hStep)*frameHStep*8.0 + 2.0*floor(textureCoordinate.x/ustep)*frameHStep;
    		mediump vec4 p0 = texture2D(inputImageTexture, vec2(x, y));
    		mediump vec4 p1 = texture2D(inputImageTexture, vec2(x+2.0*wStep, y));
    		gl_FragColor = vec4(p0.g, p0.b, p1.g, p1.b);
    	}
    }
);

GPUToNV12Filter::GPUToNV12Filter():GPUToYUV420Filter(g_tonv12_fragment_shader){
    m_filter_name = "NV12Filter";
}

void GPUToNV12Filter::setFrameSize(uint32_t width, uint32_t height){
    GPUFilter::setFrameSize(width, height);
    setOutputSize(m_frame_width, m_frame_height*3/8);
    
    setFloat("frameHStep", 1.0/(m_frame_height-1));
    setFloat("frameWStep", 1.0/(m_frame_width-1));
    //setFloat("wStep", 1.0/(m_frame_width/4-1));
    setFloat("hStep", 1.0/(m_frame_height*3/8-1));
}

////////////////////////////////////////////////////////////
//////////////////// GPUYUV420PreciseOutput /////////////////
#pragma --mark "YUV420PreciseOutput"
const static char* g_u_fragment_shader = SHADER_STRING(
    varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;

    void main()
    {
        mediump vec4 color = texture2D(inputImageTexture, textureCoordinate);
        gl_FragColor = vec4(color.g, color.g, color.b, 1.0);
    }
);

const static char* g_v_fragment_shader = SHADER_STRING(
    varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;

    void main()
    {
        mediump vec4 color = texture2D(inputImageTexture, textureCoordinate);
        gl_FragColor = vec4(color.b, color.g, color.b, 1.0);
    }
);

GPUYUV420PreciseOutput::GPUYUV420PreciseOutput():
m_u_filter(g_u_fragment_shader),
m_v_filter(g_v_fragment_shader){
    m_size = 0;
    m_bytebuffer = NULL;
    m_frame_get = true;
}

void GPUYUV420PreciseOutput::newFrame(){
    m_u_filter.render();
    m_u_filter.m_outbuffer->lock();
    
    m_v_filter.render();
    m_v_filter.m_outbuffer->lock();
    
    m_frame_get = false;
}

unsigned char * GPUYUV420PreciseOutput::getBuffer(unsigned char* buffer, uint32_t size){
    if (m_frame_get) {
        err_log("Visionin Error: RawData has been get out!");
        return NULL;
    }
    
    unsigned char* copy_buffer = buffer;
    if (buffer==NULL || size<m_size)
    {
        copy_buffer = m_bytebuffer;
    }
    GPUContext* context = GPUContext::shareInstance();
    context->makeCurrent();
    
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_inputbuffer->m_texture);
    // 分别拷贝y和uv
    glReadPixels(0, 0, m_inputbuffer->m_width, m_inputbuffer->m_height, GL_RGBA, GL_UNSIGNED_BYTE, copy_buffer);
    m_u_filter.m_outbuffer->activeBuffer();
    glReadPixels(0, 0, m_inputbuffer->m_width/2, m_inputbuffer->m_height/2, GL_LUMINANCE, GL_UNSIGNED_BYTE, copy_buffer+m_inputbuffer->m_width*m_inputbuffer->m_height);
    m_v_filter.m_outbuffer->activeBuffer();
    glReadPixels(0, 0, m_inputbuffer->m_width/2, m_inputbuffer->m_height/2, GL_LUMINANCE, GL_UNSIGNED_BYTE, copy_buffer+m_inputbuffer->m_width*m_inputbuffer->m_height*5/4);
    m_inputbuffer->unlock();
    m_u_filter.m_outbuffer->unlock();
    m_v_filter.m_outbuffer->unlock();
    // 标记已经读取
    m_frame_get = true;
    return copy_buffer;
}

void GPUYUV420PreciseOutput::setInputFrameBuffer(GPUFrameBuffer* buffer, int location){
    if (!m_frame_get) {
        m_inputbuffer->unlock();
        m_u_filter.m_outbuffer->unlock();
        m_v_filter.m_outbuffer->unlock();
    }
    
    m_inputbuffer = buffer;
    m_inputbuffer->lock();
    m_u_filter.setInputFrameBuffer(buffer);
    m_v_filter.setInputFrameBuffer(buffer);
    if (m_bytebuffer==NULL) {
        m_size = buffer->m_width*buffer->m_height*4;
        m_bytebuffer = new unsigned char[m_size];
        err_log("Visionin: RawOutput alloc buffer[%d]", m_size);
        m_u_filter.setOutputSize(buffer->m_width/2, buffer->m_height/2);
        m_v_filter.setOutputSize(buffer->m_width/2, buffer->m_height/2);
    }
    else if(m_size != buffer->m_width*buffer->m_height*4){
        delete []m_bytebuffer;
        m_size = buffer->m_width*buffer->m_height*4;
        m_bytebuffer = new unsigned char[m_size];
        err_log("Visionin: RawOutput realloc buffer[%d]", m_size);
        m_u_filter.setOutputSize(buffer->m_width/2, buffer->m_height/2);
        m_v_filter.setOutputSize(buffer->m_width/2, buffer->m_height/2);
    }
}

GPUYUV420PreciseOutput::~GPUYUV420PreciseOutput(){
    if (m_bytebuffer!=NULL) {
        delete []m_bytebuffer;
        m_bytebuffer = NULL;
    }
}

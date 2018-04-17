/**
 * file :	GPUIOSView.mm
 * author :	Rex
 * email : rex@labjk.com
 * create :	2016-07-08 16:08
 * func :
 * history:
 */

#import "GPUIOSView.h"
#import <OpenGLES/EAGLDrawable.h>
#import <QuartzCore/QuartzCore.h>
#import <AVFoundation/AVFoundation.h>
#include "GPU.h"
#include "GPUContext.h"
#include "GPUIOSBuffer.h"

extern "C"{
extern EAGLContext* currentEAGLContext();
}

GPUIOSView::GPUIOSView(CGRect rect){
    m_view = [[GPUUIView alloc] initWithFrame:rect];
}

void GPUIOSView::newFrame(){
    [m_view newFrame];
}

void GPUIOSView::setInputFrameBuffer(GPUFrameBuffer* buffer, int location){
    buffer->lock();
    [m_view setInputFrameBuffer:(GPUIOSFrameBuffer*)buffer];
}

void GPUIOSView::setOutputRotation(gpu_rotation_t rotation){
    m_rotation = rotation;
    [m_view setRotation:rotation];
}

#pragma mark -
#pragma mark Private methods and instance variables

@interface GPUUIView ()
{
    GPUIOSFrameBuffer *inputFramebufferForDisplay;
    GLuint displayRenderbuffer, displayFramebuffer;
    
    GPUProgram *displayProgram;
    GLint displayPositionAttribute, displayTextureCoordinateAttribute;
    GLint displayInputTextureUniform;

    CGSize inputImageSize;
    GLfloat imageVertices[8];

    CGSize boundsSizeAtFrameBufferEpoch;
}

@property (assign, nonatomic) NSUInteger aspectRatio;

// Initialization and teardown
- (void)commonInit;

// Managing the display FBOs
- (void)createDisplayFramebuffer;
- (void)destroyDisplayFramebuffer;

// Handling fill mode
- (void)recalculateViewGeometry;

@end

@implementation GPUUIView

@synthesize aspectRatio;
@synthesize sizeInPixels = _sizeInPixels;
@synthesize fillMode = _fillMode;
@synthesize enabled;

#pragma mark -
#pragma mark Initialization and teardown

+ (Class)layerClass 
{
	return [CAEAGLLayer class];
}

- (id)initWithFrame:(CGRect)frame
{
    if (!(self = [super initWithFrame:frame]))
    {
		return nil;
    }
    
    [self commonInit];
    return self;
}

-(id)initWithCoder:(NSCoder *)coder
{
	if (!(self = [super initWithCoder:coder])) 
    {
        return nil;
	}

    [self commonInit];
	return self;
}

- (void)commonInit;
{
    // Set scaling to account for Retina display	
    if ([self respondsToSelector:@selector(setContentScaleFactor:)])
    {
        self.contentScaleFactor = [[UIScreen mainScreen] scale];
    }

    GPUCheckGlError("IOSView init");
    displayProgram = new GPUProgram(GPUFilter::g_vertext_shader[0], GPUFilter::g_fragment_shader);
    _rotation = GPUNoRotation;
    displayPositionAttribute = displayProgram->attributeIndex("position");
    displayTextureCoordinateAttribute = displayProgram->attributeIndex("inputTextureCoordinate");
    displayInputTextureUniform = displayProgram->uniformIndex("inputImageTexture");
    displayProgram->link();
    glEnableVertexAttribArray(displayPositionAttribute);
    glEnableVertexAttribArray(displayTextureCoordinateAttribute);
    
    self.opaque = YES;
    self.hidden = NO;
    CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
    // CALayer 默认是透明的，必须将它设为不透明才能让其可见
    eaglLayer.opaque = YES;
    // 设置描绘属性，在这里设置不维持渲染内容以及颜色格式为 RGBA8
    eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
    
    GPUCheckGlError("IOSView init");
    self.enabled = YES;
}

- (void)layoutSubviews {
    GPUContext* context = GPUContext::shareInstance();
    context->glContextLock();
    
    [super layoutSubviews];
    // The frame buffer needs to be trashed and re-created when the view size changes.
    if (!CGSizeEqualToSize(self.bounds.size, boundsSizeAtFrameBufferEpoch) &&
        !CGSizeEqualToSize(self.bounds.size, CGSizeZero)) {
        
        [self destroyDisplayFramebuffer];
        [self createDisplayFramebuffer];
    } else if (!CGSizeEqualToSize(self.bounds.size, CGSizeZero)) {
        [self recalculateViewGeometry];
    }
    context->glContextUnlock();
}

- (void)dealloc
{
    [self destroyDisplayFramebuffer];
    delete displayProgram;
}

#pragma mark -
#pragma mark Managing the display FBOs

- (void)createDisplayFramebuffer;
{
    glGenFramebuffers(1, &displayFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, displayFramebuffer);
	
    glGenRenderbuffers(1, &displayRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, displayRenderbuffer);
	
    [currentEAGLContext() renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer*)self.layer];
	
    GLint backingWidth, backingHeight;

    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &backingWidth);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &backingHeight);
    
    if ( (backingWidth == 0) || (backingHeight == 0) )
    {
        [self destroyDisplayFramebuffer];
        return;
    }
    
    _sizeInPixels.width = (CGFloat)backingWidth;
    _sizeInPixels.height = (CGFloat)backingHeight;

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, displayRenderbuffer);
	
    __unused GLuint framebufferCreationStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    NSAssert(framebufferCreationStatus == GL_FRAMEBUFFER_COMPLETE, @"Failure with display framebuffer generation for display of size: %f, %f", self.bounds.size.width, self.bounds.size.height);
    boundsSizeAtFrameBufferEpoch = self.bounds.size;
    
    [self recalculateViewGeometry];
}

- (void)destroyDisplayFramebuffer;
{
    GPUContext::shareInstance()->makeCurrent();

    if (displayFramebuffer)
	{
		glDeleteFramebuffers(1, &displayFramebuffer);
		displayFramebuffer = 0;
	}
	
	if (displayRenderbuffer)
	{
		glDeleteRenderbuffers(1, &displayRenderbuffer);
		displayRenderbuffer = 0;
	}
}

- (void)activeBuffer;
{
    if (!displayFramebuffer)
    {
        [self createDisplayFramebuffer];
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, displayFramebuffer);
    glViewport(0, 0, (GLint)_sizeInPixels.width, (GLint)_sizeInPixels.height);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

- (void)newFrame;
{
    GPUContext* context = GPUContext::shareInstance();
    context->glContextLock();
    context->makeCurrent();
    
    context->setActiveProgram(displayProgram);
    [self activeBuffer];
    
    inputFramebufferForDisplay->activeTexture(GL_TEXTURE4);
    glUniform1i(displayInputTextureUniform, 4);
    
    GPUVertexBuffer* coors_buffer = GPUVertexBufferCache::shareInstance()->getVertexBuffer();
    coors_buffer->activeBuffer(displayTextureCoordinateAttribute, [GPUUIView textureCoordinatesForRotation:_rotation]);
    
    // vbo
    GPUVertexBuffer* vertex_buffer = GPUVertexBufferCache::shareInstance()->getVertexBuffer();
    vertex_buffer->activeBuffer(displayPositionAttribute, imageVertices);
    
    //    glEnableVertexAttribArray(displayPositionAttribute);
    //    glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
    //    glBufferSubData(GL_ARRAY_BUFFER, 0, 32, imageVertices);
    //    glVertexAttribPointer(displayPositionAttribute, 2, GL_FLOAT, 0, 8, 0);
    //    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //
    //    glEnableVertexAttribArray(displayTextureCoordinateAttribute);
    //    glBindBuffer(GL_ARRAY_BUFFER, vboIds[1]);
    //    glBufferSubData(GL_ARRAY_BUFFER, 0, 32, [GPUUIView textureCoordinatesForRotation:_rotation]);
    //    glVertexAttribPointer(displayTextureCoordinateAttribute, 2, GL_FLOAT, 0, 8, 0);
    //    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    //glVertexAttribPointer(displayPositionAttribute, 2, GL_FLOAT, 0, 0, imageVertices);
    //glVertexAttribPointer(displayTextureCoordinateAttribute, 2, GL_FLOAT, 0, 0, [GPUUIView textureCoordinatesForRotation:_rotation]);
    //
    //glVertexAttribPointer(displayTextureCoordinateAttribute, 2, GL_FLOAT, 0, 0, [GPUImageView textureCoordinatesForRotation:inputRotation]);
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    [self presentFramebuffer];
    glFlush();
    
    context->glContextUnlock();
    coors_buffer->unLock();
    vertex_buffer->unLock();
    
    inputFramebufferForDisplay->unlock();
    //inputFramebufferForDisplay = NULL;
}

- (void)presentFramebuffer;
{
    glBindRenderbuffer(GL_RENDERBUFFER, displayRenderbuffer);
    GPUContext::shareInstance()->swapBuffer();
}

#pragma mark -
#pragma mark Accessors

- (void)setFillMode:(gpu_fill_mode_t)newValue;
{
    _fillMode = newValue;
    [self recalculateViewGeometry];
}

-(void)setInputFrameBuffer:(GPUIOSFrameBuffer*)frameBuffer{
    inputFramebufferForDisplay = frameBuffer;
    inputImageSize = CGSizeMake(frameBuffer->m_width, frameBuffer->m_height);
    [self recalculateViewGeometry];
}

#pragma mark -
#pragma mark Handling fill mode

- (void)recalculateViewGeometry;
{
    CGFloat heightScaling, widthScaling;
    
    CGSize currentViewSize = self.bounds.size;
    
    //    CGFloat imageAspectRatio = inputImageSize.width / inputImageSize.height;
    //    CGFloat viewAspectRatio = currentViewSize.width / currentViewSize.height;
    
    CGRect insetRect = AVMakeRectWithAspectRatioInsideRect(inputImageSize, self.bounds);
    
    switch(_fillMode)
    {
        case GPUFillModeStretch:
        {
            widthScaling = 1.0;
            heightScaling = 1.0;
        }; break;
        case GPUFillModePreserveAspectRatio:
        {
            widthScaling = insetRect.size.width / currentViewSize.width;
            heightScaling = insetRect.size.height / currentViewSize.height;
        }; break;
        case GPUFillModePreserveAspectRatioAndFill:
        {
            //            CGFloat widthHolder = insetRect.size.width / currentViewSize.width;
            widthScaling = currentViewSize.height / insetRect.size.height;
            heightScaling = currentViewSize.width / insetRect.size.width;
        }; break;
    }
    
    imageVertices[0] = -widthScaling;
    imageVertices[1] = -heightScaling;
    imageVertices[2] = widthScaling;
    imageVertices[3] = -heightScaling;
    imageVertices[4] = -widthScaling;
    imageVertices[5] = heightScaling;
    imageVertices[6] = widthScaling;
    imageVertices[7] = heightScaling;
}

+ (const GLfloat *)textureCoordinatesForRotation:(gpu_rotation_t)rotationMode;
{
//    static const GLfloat noRotationTextureCoordinates[] = {
//        0.0f, 0.0f,
//        1.0f, 0.0f,
//        0.0f, 1.0f,
//        1.0f, 1.0f,
//    };
    
    static const GLfloat noRotationTextureCoordinates[] = {
        0.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
    };

    static const GLfloat rotateRightTextureCoordinates[] = {
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
    };

    static const GLfloat rotateLeftTextureCoordinates[] = {
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
    };
        
    static const GLfloat verticalFlipTextureCoordinates[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
    };
    
    static const GLfloat horizontalFlipTextureCoordinates[] = {
        1.0f, 1.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
    };
    
    static const GLfloat rotateRightVerticalFlipTextureCoordinates[] = {
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,
    };
    
    static const GLfloat rotateRightHorizontalFlipTextureCoordinates[] = {
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
    };

    static const GLfloat rotate180TextureCoordinates[] = {
        1.0f, 0.0f,
        0.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
    };
    
    switch(rotationMode)
    {
        case GPUNoRotation: return noRotationTextureCoordinates;
        case GPURotateLeft: return rotateLeftTextureCoordinates;
        case GPURotateRight: return rotateRightTextureCoordinates;
        case GPUFlipVertical: return verticalFlipTextureCoordinates;
        case GPUFlipHorizonal: return horizontalFlipTextureCoordinates;
        case GPURotateRightFlipVertical: return rotateRightVerticalFlipTextureCoordinates;
        case GPURotateRightFlipHorizontal: return rotateRightHorizontalFlipTextureCoordinates;
        case GPURotate180: return rotate180TextureCoordinates;
    }
}

@end


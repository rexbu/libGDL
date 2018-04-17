/**
 * file :	GPUPicture.mm
 * author :	Rex
 * email : rex@labjk.com
 * create :	2016-08-01 16:39
 * func : 
 * history:
 */

#include "GPUPicture.h"
#include <assert.h>
#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>

GPUPicture::GPUPicture(uint8_t * data, uint32_t size){
    NSData* nsdata = [NSData dataWithBytes:data length:size];
    UIImage* image = [UIImage imageWithData:nsdata];
    m_exist = true;
    if (image==nil) {
        m_exist = false;
        return;
    }
    
    m_option = GPUFrameBuffer::defaultFrameOption();
    init(image.CGImage);
}

GPUPicture::GPUPicture(void* cgImageRef){
    m_exist = true;
    if (cgImageRef==NULL) {
        m_exist = false;
        return;
    }
    
    m_option = GPUFrameBuffer::defaultFrameOption();
    init(cgImageRef);
}

GPUPicture::GPUPicture(const char* file, const char* type){
//    NSData *imageData = [[NSData alloc] initWithContentsOfURL:url];
//    UIImage *inputImage = [[UIImage alloc] initWithData:imageData];
    NSString* nsfile = [[NSBundle mainBundle] pathForResource:[NSString stringWithUTF8String:file] ofType:[NSString stringWithUTF8String:type]];
    UIImage* image = [[UIImage alloc] initWithContentsOfFile:nsfile];
    m_exist = true;
    if (image==nil) {
        m_exist = false;
        return;
    }
    
    m_option = GPUFrameBuffer::defaultFrameOption();
    init(image.CGImage);
}

GPUPicture::GPUPicture(const char* path){
    UIImage* image = [[UIImage alloc] initWithContentsOfFile:[NSString stringWithUTF8String:path]];
    m_exist = true;
    if (image==nil) {
        m_exist = false;
        return;
    }
    
    m_option = GPUFrameBuffer::defaultFrameOption();
    init(image.CGImage);
}

void GPUPicture::init(void* image){
    CGImageRef newImageSource = (CGImageRef)image;
    hasProcessedImage = NO;
    
    // TODO: Dispatch this whole thing asynchronously to move image loading off main thread
    CGFloat widthOfImage = CGImageGetWidth(newImageSource);
    CGFloat heightOfImage = CGImageGetHeight(newImageSource);
    
    // If passed an empty image reference, CGContextDrawImage will fail in future versions of the SDK.
    assert( widthOfImage > 0 && heightOfImage > 0);
    
    m_image_size = {static_cast<uint32_t>(widthOfImage), static_cast<uint32_t>(heightOfImage)};
    gpu_size_t pixelSizeToUseForTexture = m_image_size;
    
    BOOL shouldRedrawUsingCoreGraphics = NO;
    
    // For now, deal with images larger than the maximum texture size by resizing to be within that limit
    gpu_size_t scaledImageSizeToFitOnGPU = GPUContext::shareInstance()->sizeFitsTextureMaxSize(m_image_size);
    
    if (!(scaledImageSizeToFitOnGPU.width==m_image_size.width && scaledImageSizeToFitOnGPU.height==m_image_size.height))
    {
        m_image_size = scaledImageSizeToFitOnGPU;
        pixelSizeToUseForTexture = m_image_size;
        shouldRedrawUsingCoreGraphics = YES;
    }
    
    GLubyte *imageData = NULL;
    CFDataRef dataFromImageDataProvider = NULL;
    GLenum format = GL_BGRA;
    BOOL isLitteEndian = YES;
    BOOL alphaFirst = NO;
    BOOL premultiplied = NO;
    
    if (!shouldRedrawUsingCoreGraphics) {
        /* Check that the memory layout is compatible with GL, as we cannot use glPixelStore to
         * tell GL about the memory layout with GLES.
         */
        if (CGImageGetBytesPerRow(newImageSource) != CGImageGetWidth(newImageSource) * 4 ||
            CGImageGetBitsPerPixel(newImageSource) != 32 ||
            CGImageGetBitsPerComponent(newImageSource) != 8)
        {
            shouldRedrawUsingCoreGraphics = YES;
        } else {
            /* Check that the bitmap pixel format is compatible with GL */
            CGBitmapInfo bitmapInfo = CGImageGetBitmapInfo(newImageSource);
            if ((bitmapInfo & kCGBitmapFloatComponents) != 0) {
                /* We don't support float components for use directly in GL */
                shouldRedrawUsingCoreGraphics = YES;
            } else {
                CGBitmapInfo byteOrderInfo = bitmapInfo & kCGBitmapByteOrderMask;
                if (byteOrderInfo == kCGBitmapByteOrder32Little) {
                    /* Little endian, for alpha-first we can use this bitmap directly in GL */
                    CGImageAlphaInfo alphaInfo = (CGImageAlphaInfo)((uint32_t)bitmapInfo & (uint32_t)kCGBitmapAlphaInfoMask);
                    if (alphaInfo != kCGImageAlphaPremultipliedFirst && alphaInfo != kCGImageAlphaFirst &&
                        alphaInfo != kCGImageAlphaNoneSkipFirst) {
                        shouldRedrawUsingCoreGraphics = YES;
                    }
                } else if (byteOrderInfo == kCGBitmapByteOrderDefault || byteOrderInfo == kCGBitmapByteOrder32Big) {
                    isLitteEndian = NO;
                    /* Big endian, for alpha-last we can use this bitmap directly in GL */
                    CGImageAlphaInfo alphaInfo = (CGImageAlphaInfo)((uint32_t)bitmapInfo & (uint32_t)kCGBitmapAlphaInfoMask);
                    if (alphaInfo != kCGImageAlphaPremultipliedLast && alphaInfo != kCGImageAlphaLast &&
                        alphaInfo != kCGImageAlphaNoneSkipLast) {
                        shouldRedrawUsingCoreGraphics = YES;
                    } else {
                        /* Can access directly using GL_RGBA pixel format */
                        premultiplied = alphaInfo == kCGImageAlphaPremultipliedLast || alphaInfo == kCGImageAlphaPremultipliedLast;
                        alphaFirst = alphaInfo == kCGImageAlphaFirst || alphaInfo == kCGImageAlphaPremultipliedFirst;
                        format = GL_RGBA;
                    }
                }
            }
        }
    }
    
    //    CFAbsoluteTime elapsedTime, startTime = CFAbsoluteTimeGetCurrent();
    
    if (shouldRedrawUsingCoreGraphics)
    {
        // For resized or incompatible image: redraw
        imageData = (GLubyte *) calloc(1, (int)pixelSizeToUseForTexture.width * (int)pixelSizeToUseForTexture.height * 4);
        
        CGColorSpaceRef genericRGBColorspace = CGColorSpaceCreateDeviceRGB();
        
        CGContextRef imageContext = CGBitmapContextCreate(imageData, (size_t)pixelSizeToUseForTexture.width, (size_t)pixelSizeToUseForTexture.height, 8, (size_t)pixelSizeToUseForTexture.width * 4, genericRGBColorspace,  kCGBitmapByteOrder32Little | kCGImageAlphaPremultipliedFirst);
        //        CGContextSetBlendMode(imageContext, kCGBlendModeCopy); // From Technical Q&A QA1708: http://developer.apple.com/library/ios/#qa/qa1708/_index.html
        CGContextDrawImage(imageContext, CGRectMake(0.0, 0.0, pixelSizeToUseForTexture.width, pixelSizeToUseForTexture.height), newImageSource);
        CGContextRelease(imageContext);
        CGColorSpaceRelease(genericRGBColorspace);
        isLitteEndian = YES;
        alphaFirst = YES;
        premultiplied = YES;
    }
    else
    {
        // Access the raw image bytes directly
        dataFromImageDataProvider = CGDataProviderCopyData(CGImageGetDataProvider(newImageSource));
        imageData = (GLubyte *)CFDataGetBytePtr(dataFromImageDataProvider);
    }
    
    m_option.format = format;
    setPixel(imageData, pixelSizeToUseForTexture.width, pixelSizeToUseForTexture.height);
    
    if (shouldRedrawUsingCoreGraphics)
    {
        free(imageData);
    }
    else
    {
        if (dataFromImageDataProvider)
        {
            CFRelease(dataFromImageDataProvider);
        }
    }
}

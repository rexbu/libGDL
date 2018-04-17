//
//  EdgeViewController.m
//  example
//
//  Created by Visionin on 17/5/25.
//  Copyright © 2017年 Rex. All rights reserved.
//

#import "EdgeViewController.h"
#include "GPU.h"
#import "GPUIOSView.h"
#import "GPUIOSBuffer.h"

@interface EdgeViewController (){
    GPUPicture*     pic_origin;
    GPUPicture*     pic;
    GPUPicture*     pic2;
    
    GPUFilter*      medianFilter;
    GPUFilter*      convolution9Filter;
    GPUFilter*      binaryFilter;
    GPUFilter*      saltFilter;
    GPUFilter*      smoothFilter;
    
    GPUIOSView*     view;
}

@end

@implementation EdgeViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    bs_log_init("stdout");
    
    GPUIOSBufferCache::shareInstance();
//    NSString* path = [[NSBundle mainBundle] pathForResource:@"me" ofType:@"jpeg"];
//    pic_origin = new GPUPicture(path.UTF8String);
//    smoothFilter = new GPUSmoothFilter();
//    smoothFilter->setExtraParameter(0.9);
//    pic_origin->addTarget(smoothFilter);
//    pic_origin->processImage();
//    glFinish();
//    GPUIOSFrameBuffer* smooth_buffer = (GPUIOSFrameBuffer*)smoothFilter->m_outbuffer;
//        smooth_buffer->lock();
//    CVPixelBufferRef pixelBuffer = smooth_buffer->getPixelBuffer();
//    CIImage* inputImage = [CIImage imageWithCVPixelBuffer:pixelBuffer];
    
    medianFilter = new GPUMedianFilter();
    
    view = new GPUIOSView(self.view.bounds);
    [self.view addSubview:view->uiview()];
    
    CIImage *inputImage = [[CIImage alloc] initWithImage:[UIImage imageNamed:@"z1.jpeg"]];
    CIContext *context = [CIContext contextWithOptions:nil];
    
    // 灰度图
    CIFilter* grayFilter = [CIFilter filterWithName:@"CIMaximumComponent"];
    [grayFilter setValue:inputImage forKey:kCIInputImageKey];
    // 高斯边缘检测
    CIFilter *blurFilter = [CIFilter filterWithName:@"CIGaussianBlur"];
    [blurFilter setValue:grayFilter.outputImage forKey:kCIInputImageKey];
    [blurFilter setValue:[NSNumber numberWithFloat:2] forKey:@"inputRadius"];
    
    CIFilter *blurFilter2 = [CIFilter filterWithName:@"CIGaussianBlur"];
    [blurFilter2 setValue:grayFilter.outputImage forKey:kCIInputImageKey];
    [blurFilter2 setValue:[NSNumber numberWithFloat:0.9] forKey:@"inputRadius"];
    
    CIFilter* source = [CIFilter filterWithName:@"CIDivideBlendMode"];
    [source setValue:blurFilter.outputImage forKey:@"inputImage"];
    [source setValue:blurFilter2.outputImage forKey:@"inputBackgroundImage"];
    // 对边缘图进行平滑
    CIFilter *blurFilter3 = [CIFilter filterWithName:@"CIGaussianBlur"];
    [blurFilter3 setValue:source.outputImage forKey:kCIInputImageKey];
    [blurFilter3 setValue:[NSNumber numberWithFloat:0.6] forKey:@"inputRadius"];
    
    CIFilter *edgeConstColor = [CIFilter filterWithName:@"CIConstantColorGenerator"];
    [edgeConstColor setValue:[CIColor colorWithRed:0.99 green:0.99 blue:0.99] forKey:@"inputColor"];
    CIFilter* edgeSubFilter = [CIFilter filterWithName:@"CIDivideBlendMode"];
    [edgeSubFilter setValue:edgeConstColor.outputImage forKey:@"inputImage"];
    [edgeSubFilter setValue:blurFilter3.outputImage forKey:@"inputBackgroundImage"];
    
    // 差分背景
    CIFilter *blurFilter5 = [CIFilter filterWithName:@"CIGaussianBlur"];
    [blurFilter5 setValue:grayFilter.outputImage forKey:kCIInputImageKey];
    [blurFilter5 setValue:[NSNumber numberWithFloat:0.6] forKey:@"inputRadius"];
    
    CIFilter *constColor = [CIFilter filterWithName:@"CIConstantColorGenerator"];
    [constColor setValue:[CIColor colorWithRed:0.25 green:0.25 blue:0.25] forKey:@"inputColor"];
    CIFilter* grayBack = [CIFilter filterWithName:@"CIDivideBlendMode"];
    [grayBack setValue:constColor.outputImage forKey:@"inputImage"];
    [grayBack setValue:grayFilter.outputImage forKey:@"inputBackgroundImage"];
    
    CIFilter *constColor2 = [CIFilter filterWithName:@"CIConstantColorGenerator"];
    [constColor2 setValue:[CIColor colorWithRed:0.1 green:0.1 blue:0.1] forKey:@"inputColor"];
    CIFilter* blendBack = [CIFilter filterWithName:@"CIOverlayBlendMode"];
    [blendBack setValue:constColor2.outputImage forKey:@"inputImage"];
    [blendBack setValue:grayBack.outputImage forKey:@"inputBackgroundImage"];
    // 模糊差分后的背景
    CIFilter *blurFilter4 = [CIFilter filterWithName:@"CIGaussianBlur"];
    [blurFilter4 setValue:blendBack.outputImage forKey:kCIInputImageKey];
    [blurFilter4 setValue:[NSNumber numberWithFloat:2] forKey:@"inputRadius"];
    
//    
//    
//    CIFilter *whiteBackColor = [CIFilter filterWithName:@"CIConstantColorGenerator"];
//    [whiteBackColor setValue:[CIColor whiteColor] forKey:@"inputColor"];
//    CIFilter* background1 = [CIFilter filterWithName:@"CIMultiplyBlendMode"];
//    [background1 setValue:whiteBackColor.outputImage forKey:@"inputImage"];
//    [background1 setValue:background.outputImage forKey:@"inputBackgroundImage"];
//    
//    CIFilter *constBackColor = [CIFilter filterWithName:@"CIConstantColorGenerator"];
//    [constBackColor setValue:[CIColor colorWithRed:0.94 green:0.91 blue:0.88] forKey:@"inputColor"];
//    CIFilter* background2 = [CIFilter filterWithName:@"CIMultiplyBlendMode"];
//    [background2 setValue:constBackColor.outputImage forKey:@"inputImage"];
//    [background2 setValue:background.outputImage forKey:@"inputBackgroundImage"];
    
    CIFilter* shaper = [CIFilter filterWithName:@"CISharpenLuminance"];
    [shaper setValue:edgeSubFilter.outputImage forKey:@"inputImage"];
    [shaper setValue:@80.0 forKey:@"inputSharpness"];
    
    CIImage* outImage = shaper.outputImage;
    CGImageRef cgImage = [context createCGImage:outImage fromRect:[inputImage extent]];
    pic = new GPUPicture(cgImage);
    pic->addTarget(medianFilter);
    
    convolution9Filter = new GPUHConvolution9Filter();
    binaryFilter = new GPUBinaryFilter();
    binaryFilter->setExtraParameter(0.75);
    
    medianFilter->addTarget(convolution9Filter);
    convolution9Filter->addTarget(binaryFilter);
    
    pic->processImage();
    glFinish();
    
    // 调试view
    CGImageRef debugImage = [context createCGImage:blendBack.outputImage fromRect:[inputImage extent]];
    UIImageView* imageView = [[UIImageView alloc] initWithFrame:self.view.bounds];
    imageView.image = [UIImage imageWithCGImage:debugImage];
    // [self.view addSubview:imageView];
    
    
    GPUIOSFrameBuffer* framebuffer = (GPUIOSFrameBuffer*)binaryFilter->m_outbuffer;
    CIImage* backImage = [CIImage imageWithCVPixelBuffer:framebuffer->getPixelBuffer()];
    
    CIFilter* background = [CIFilter filterWithName:@"CIMultiplyBlendMode"];
    [background setValue:blurFilter4.outputImage forKey:@"inputImage"];
    [background setValue:backImage forKey:@"inputBackgroundImage"];
    
    
    CGImageRef finishImage = [context createCGImage:background.outputImage fromRect:[inputImage extent]];
    
    pic2 = new GPUPicture(finishImage);
    saltFilter = new GPUSaltFilter();
    
    pic2->addTarget(saltFilter);
    saltFilter->addTarget(view);
    pic2->processImage();
    // 循环画
//    [NSTimer scheduledTimerWithTimeInterval:0.1 target:self selector:@selector(newFrame) userInfo:nil repeats:NO];
}

-(void) newFrame{
    pic2->processImage();
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
    
}

@end

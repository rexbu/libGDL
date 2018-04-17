//
//  PixelTest.m
//  DeepExample
//
//  Created by Rex on 2017/12/17.
//  Copyright © 2017年 Rex. All rights reserved.
//

#import "PixelTest.h"

@implementation PixelTest

-(id)initWithView:(UIView*)uiview{
    uint8_t pixel[] = {
        60,10,10,255, 60,0,10,255,
        20,0,0,255, 80,20,0,255,
        60,0,0,255, 20,0,0,255,
        50,20,0,255, 10,50,0,255,
    };
    
    pic = new GPUPicture(pixel, 2, 4);
    // pic = new GPUPicture(path.UTF8String);
    
    input = new GPUConvShape(4, 3, 1);
    pic->addTarget(input);
    output = new GPURawOutput();
    
    layer = new GPUConvLayer(3, 4);
    float layer_kernel[3*3*3*4];
    for (int i=0; i<sizeof(layer_kernel)/sizeof(float); i++){
        layer_kernel[i] =0.1*(i/27+1);
    }
    layer->setConvKernel(layer_kernel, sizeof(layer_kernel)/sizeof(float));
    float conv_bias[] = {0.1, 0.5, 0.3, 0.4};
    layer->setBias(conv_bias, 4);
    input->addTarget(layer);
    
    pool = new GPUPoolLayer(4, 2);
    layer->addTarget(pool);
    
    layer2 = new GPUConvLayer(3, 4);
    float layer2_kernel[3*3*4*4];
    for (int i=0; i<sizeof(layer2_kernel)/sizeof(float); i++){
        layer2_kernel[i] =0.1*(i/36+1);
    }
    layer2->setConvKernel(layer2_kernel, sizeof(layer2_kernel)/sizeof(float));
    float conv2_bias[] = {0.1, 0.5, 0.3, 0.4};
    layer2->setBias(conv2_bias, 4);
    pool->addTarget(layer2);
    
    pool2 = new GPUPoolLayer(4, 2);
    layer2->addTarget(pool2);
    
    full = new GPUFullConnLayer(2);
    float wei[] = {0.1f, 0.2f, 0.1f, 0.05f, 0.05f, 0.1f, 0.1f, 0.05f};
    float bais[] = {0.5f, 0.1f};
    full->setWeights(wei, bais, 4, 2);
    pool2->addTarget(full);
    
    GPUIOSView* view = new GPUIOSView(CGRectMake(0, 0, 300, 300));
    [uiview addSubview:view->uiview()];
    full->addTarget(view);
    
    return [super init];
}

-(void)process{
    pic->processImage();
    layer->newFrame();
    pool->newFrame();
    layer2->newFrame();
    pool2->newFrame();
    full->newFrame();
}
@end

//
//  ModelTest.m
//  DeepExample
//
//  Created by Rex on 2017/12/17.
//  Copyright © 2017年 Rex. All rights reserved.
//

#import "ModelTest.h"

@implementation ModelTest

-(id)initWithModel:(NSString*)path  view:(UIView*)view{
    //session = new GDLSession(28, path.UTF8String);
    session = new GDLSession(path.UTF8String);
    uint8_t number[] = {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                   255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                   255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                   255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                   255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 244, 240, 255, 250, 222, 217, 231, 253, 255, 255, 255,
                   255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 252, 245, 220, 216, 215, 141, 95, 84, 185, 113, 0, 0, 111, 248, 255, 255, 255,
                   255, 255, 255, 255, 255, 255, 255, 247, 210, 191, 158, 112, 91, 38, 16, 19, 8, 0, 32, 94, 39, 1, 42, 163, 254, 255, 255, 255,
                   255, 255, 255, 255, 255, 255, 255, 217, 50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 131, 180, 164, 187, 215, 250, 255, 255, 255, 255,
                   255, 255, 255, 255, 255, 255, 255, 238, 66, 0, 0, 0, 0, 0, 62, 100, 23, 16, 213, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                   255, 255, 255, 255, 255, 255, 255, 255, 186, 111, 127, 42, 0, 50, 208, 255, 196, 128, 225, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                   255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 116, 0, 116, 255, 255, 255, 255, 252, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                   255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 157, 0, 48, 232, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                   255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 243, 84, 0, 133, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                   255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 230, 47, 0, 83, 146, 239, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                   255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 187, 29, 0, 0, 115, 235, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                   255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 205, 74, 0, 0, 89, 221, 255, 255, 255, 255, 255, 255, 255, 255,
                   255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 247, 167, 17, 0, 66, 218, 255, 255, 255, 255, 255, 255, 255,
                   255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 58, 0, 0, 157, 255, 255, 255, 255, 255, 255, 255,
                   255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 217, 150, 101, 15, 0, 15, 197, 255, 255, 255, 255, 255, 255, 255,
                   255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 232, 147, 57, 7, 0, 0, 0, 68, 230, 255, 255, 255, 255, 255, 255, 255,
                   255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 245, 168, 65, 0, 0, 0, 0, 40, 143, 231, 255, 255, 255, 255, 255, 255, 255, 255,
                   255, 255, 255, 255, 255, 255, 255, 255, 236, 200, 91, 14, 0, 0, 0, 47, 139, 221, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                   255, 255, 255, 255, 255, 255, 250, 137, 55, 18, 0, 0, 0, 35, 140, 223, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                   255, 255, 255, 255, 211, 108, 40, 0, 0, 0, 0, 4, 86, 203, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                   255, 255, 255, 255, 160, 24, 0, 1, 38, 93, 112, 177, 240, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                   255, 255, 255, 255, 232, 211, 214, 219, 229, 245, 253, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                   255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};
    //pic = new GPUPicture([[NSBundle mainBundle] pathForResource:@"mnist" ofType:@"png"].UTF8String);
    gpu_frame_option_t option = GPUFrameBuffer::defaultFrameOption();
    option.color_format = GL_LUMINANCE;
    option.format = GL_LUMINANCE;
    pic = new GPUPicture((uint8_t*)number, 28, 28, &option);
    preview = new GPUIOSView(CGRectMake(0, 0, 300, 300));
    [view addSubview:preview->uiview()];
    
    pic->addTarget(session);
    GPUConvShape* shape = new GPUConvShape(28, 1, 2);
    session->addLayerBase(shape);
    GPUConvLayer* conv = new GPUConvLayer(5, 32);
    session->addConv(conv, "conv1");
    GPUPoolLayer* pool = new GPUPoolLayer(32, 2, CONV_POOL_MAX, 2);
    session->addLayerBase(pool);
    conv = new GPUConvLayer(5, 64);
    session->addConv(conv, "conv2");
    pool = new GPUPoolLayer(64);
    session->addLayerBase(pool);

    CPUFullConnLayer* full = new CPUFullConnLayer();
    session->addFullConv(full, "fc1");
    full = new CPUFullConnLayer(CONV_ACTIVE_NULL);
    session->addFullConv(full, "fc2");
    
    session->addTarget(preview);
    // luminance->addTarget(session);
    // session->addTarget(preview);
    
    path = [[NSBundle mainBundle] pathForResource:@"conv1" ofType:@"val"];
    session->layer(1)->setTensor(path.UTF8String);
    path = [[NSBundle mainBundle] pathForResource:@"pool1" ofType:@"val"];
    session->layer(2)->setTensor(path.UTF8String);
    path = [[NSBundle mainBundle] pathForResource:@"conv2" ofType:@"val"];
    session->layer(3)->setTensor(path.UTF8String);
    path = [[NSBundle mainBundle] pathForResource:@"pool2" ofType:@"val"];
    session->layer(4)->setTensor(path.UTF8String);
    path = [[NSBundle mainBundle] pathForResource:@"fc1" ofType:@"val"];
    session->layer(5)->setTensor(path.UTF8String);
    path = [[NSBundle mainBundle] pathForResource:@"fc2" ofType:@"val"];
    session->layer(6)->setTensor(path.UTF8String);
    return [super init];
}

-(void)process{
    pic->processImage();
}
@end

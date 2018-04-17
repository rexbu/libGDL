//
//  StyleTest.m
//  DeepExample
//
//  Created by Rex on 2018/1/25.
//  Copyright © 2018年 Rex. All rights reserved.
//

#import "StyleTest.h"

@implementation StyleTest

-(id)initWithView:(UIView*)view{
    /*
    int width = 256;
    int height = 170; // 170
    NSString* pic_path = [[NSBundle mainBundle] pathForResource:@"chicago" ofType:@"val"];
    struct stat statbuff;
    stat(pic_path.UTF8String, &statbuff);
    uint8_t* p_values = (uint8_t*)calloc(1, width*height*4);
    FILE* fp = fopen(pic_path.UTF8String, "r");
    for (int i=0; i<height; i++) {
        for (int j=0; j<width; j++) {
            fread(p_values+(i*width+j)*4, 3, 1, fp);
        }
    }
    fclose(fp);
     pic = new GPUPicture(p_values, width, height);
     free(p_values);
    */
    pic = new GPUPicture([[NSBundle mainBundle] pathForResource:@"chicago" ofType:@"jpg"].UTF8String);
    
    NSString* modelpath = [[NSBundle mainBundle] pathForResource:@"style" ofType:@"model"];
    style = new GDLStyle(modelpath.UTF8String);
    // style->encoder();
    
    NSString *style_path = [[NSBundle mainBundle] pathForResource:@"udnie" ofType:@"style"];
    style->style(style_path.UTF8String);
    
    preview = new GPUIOSView(CGRectMake(0, 0, view.bounds.size.width, view.bounds.size.width/1.5));
    [view addSubview:preview->uiview()];
    
    //[self setTensor:@"pre_padding" path:[[NSBundle mainBundle] pathForResource:@"padding" ofType:@"val"]];
//    [self setTensor:@"conv1_1" path:[[NSBundle mainBundle] pathForResource:@"conv1_1" ofType:@"val"]];
//    [self setTensor:@"conv1_2" path:[[NSBundle mainBundle] pathForResource:@"conv1_2" ofType:@"val"]];
//    [self setTensor:@"pool1" path:[[NSBundle mainBundle] pathForResource:@"pool1" ofType:@"val"]];
//    [self setTensor:@"conv2_1" path:[[NSBundle mainBundle] pathForResource:@"conv2_1" ofType:@"val"]];
//    [self setTensor:@"conv2_2" path:[[NSBundle mainBundle] pathForResource:@"conv2_2" ofType:@"val"]];
//    [self setTensor:@"pool2" path:[[NSBundle mainBundle] pathForResource:@"pool2" ofType:@"val"]];
//    [self setTensor:@"pool3" path:[[NSBundle mainBundle] pathForResource:@"pool3" ofType:@"val"]];
//    [self setTensor:@"conv4_1" path:[[NSBundle mainBundle] pathForResource:@"conv4_1" ofType:@"val"]];
//    [self setTensor:@"mean" path:[[NSBundle mainBundle] pathForResource:@"mean_c" ofType:@"val"]];
//    [self setTensor:@"var" path:[[NSBundle mainBundle] pathForResource:@"sigma_c" ofType:@"val"]];
//    [self setTensor:@"adain" path:[[NSBundle mainBundle] pathForResource:@"adain" ofType:@"val"]];
//    [self setTensor:@"dconv4_1" path:[[NSBundle mainBundle] pathForResource:@"dconv4_1" ofType:@"val"]];
//    [self setTensor:@"unsample_0" path:[[NSBundle mainBundle] pathForResource:@"unsample_0" ofType:@"val"]];
//    [self setTensor:@"unsample_4" path:[[NSBundle mainBundle] pathForResource:@"unsample_4" ofType:@"val"]];
//    [self setTensor:@"unsample_6" path:[[NSBundle mainBundle] pathForResource:@"unsample_6" ofType:@"val"]];
//    [self setTensor:@"dconv1_1" path:[[NSBundle mainBundle] pathForResource:@"dconv1_1" ofType:@"val"]];
    pic->addTarget(style);
    style->addTarget(preview);
    return [super init];
}

-(void)process{
    pic->processImage();
//    NSString *docDir = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) firstObject];
//    NSString* path = [NSString stringWithFormat:@"%@/udnie.style", docDir, nil];
//    style->saveStyle(path.UTF8String);
}

-(void)setTensor:(NSString*)name path:(NSString*)path{
    GPULayerBase* layer = style->layer(name.UTF8String);
    layer->setTensor(path.UTF8String);
}
@end

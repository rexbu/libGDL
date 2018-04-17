//
//  PixelTest.h
//  DeepExample
//
//  Created by Rex on 2017/12/17.
//  Copyright © 2017年 Rex. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#include "GDLGlobal.h"
#include "GPUIOSView.h"

@interface PixelTest : NSObject{
    GPUConvShape* input;
    GPUPicture* pic;
    GPUConvLayer* layer;
    GPUPoolLayer* pool;
    GPUConvLayer* layer2;
    GPUPoolLayer* pool2;
    GPUFullConnLayer* full;
    GPURawOutput* output;
}

-(id)initWithView:(UIView*)view;
-(void)process;
@end

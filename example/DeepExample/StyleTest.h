//
//  StyleTest.h
//  DeepExample
//
//  Created by Rex on 2018/1/25.
//  Copyright © 2018年 Rex. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#include "GDLGlobal.h"
#include "GPUIOSView.h"
#include "GPUColorSpread.h"
#include "GPUPadding.h"
#include "GDLStyle.h"

@interface StyleTest : NSObject{
    GPUPicture*         pic;
    GPUColorSpread*     spread;
    GPUPadding*         padding;
    GDLStyle*           style;
    
    GPULuminanceFilter* luminance;
    GPUIOSView*         preview;
}

-(id)initWithView:(UIView*)view;
-(void)process;
@end

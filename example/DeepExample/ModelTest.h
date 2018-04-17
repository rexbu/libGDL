//
//  ModelTest.h
//  DeepExample
//
//  Created by Rex on 2017/12/17.
//  Copyright © 2017年 Rex. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "GDLSession.h"
#import "GPUIOSView.h"

@interface ModelTest : NSObject{
    GDLSession*         session;
    GPUPicture*         pic;
    GPULuminanceFilter* luminance;
    GPUIOSView*         preview;
}

-(id)initWithModel:(NSString*)path view:(UIView*)view;
-(void)process;
@end

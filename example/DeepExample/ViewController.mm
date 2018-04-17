//
//  ViewController.m
//  DeepExample
//
//  Created by Rex on 17/7/27.
//  Copyright © 2017年 Rex. All rights reserved.
//

#import "ViewController.h"
#import "PixelTest.h"
#import "ModelTest.h"
#include "GPUTest.h"
#import "StyleTest.h"

@interface ViewController (){
    PixelTest* pixel;
    ModelTest* model;
    StyleTest* style;
}

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    NSString* path = [[NSBundle mainBundle] pathForResource:@"udnie" ofType:@"jpg"];
    bs_log_init("stdout");

//    GPUTestFilter* test = new GPUTestFilter();
//    test->newFrame();
//    pixel = [[PixelTest alloc] initWithView:self.view];
//    NSTimer *timer = [NSTimer scheduledTimerWithTimeInterval:0.03 target:pixel selector:@selector(process) userInfo:nil repeats:YES];
//    model = [[ModelTest alloc] initWithModel:[[NSBundle mainBundle] pathForResource:@"mnist" ofType:@"gdl"] view:self.view];
//    NSTimer *timer = [NSTimer scheduledTimerWithTimeInterval:0.01 target:model selector:@selector(process) userInfo:nil repeats:YES];
    
    style = [[StyleTest alloc] initWithView:self.view];
    NSTimer *timer = [NSTimer scheduledTimerWithTimeInterval:0.5 target:style selector:@selector(process) userInfo:nil repeats:YES];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}


@end

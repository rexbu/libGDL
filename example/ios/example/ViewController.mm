//
//  ViewController.m
//  example
//
//  Created by Visionin on 17/5/20.
//  Copyright © 2017年 Rex. All rights reserved.
//

#import "ViewController.h"
#import "GPUIOSView.h"
#include "GPU.h"

@interface ViewController (){
    GPUPaintFilter* line;
    GPUIOSView* view;
}

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    bs_log_init("stdout");
    
    NSString* path = [[NSBundle mainBundle] pathForResource:@"kaiwen2" ofType:@"png"];
    GPUPicture* pic = new GPUPicture(path.UTF8String);
    GPUFilter* filter = new GPUFilter();
    
    pic->addTarget(filter);
    pic->processImage();
    
    line = new GPUPaintFilter();
    view = new GPUIOSView(self.view.bounds);
    [self.view addSubview:view->uiview()];
    
    //line->setOutputFrameBuffer(filter->m_outbuffer);
    line->addTarget(view);
    
    line->clear();
    gpu_colorf_t c0 = {0.0, 0.0, 0.0, 1.0};
    gpu_colorf_t c1 = {0.5, 0.5, 0.5, 0.8};
    line->setColors(c0, c1);
    line->setLineWidth(3);
    
    [self paintFace];
    line->newFrame();
    
    // 循环画
    //[NSTimer scheduledTimerWithTimeInterval:0.1 target:self selector:@selector(newFrame) userInfo:nil repeats:YES];
}
-(void)paintFace{
    NSString* face_string = [NSString stringWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"facepoints" ofType:@"json"]];
    NSData *resData = [[NSData alloc] initWithData:[face_string dataUsingEncoding:NSUTF8StringEncoding]];
    
    NSDictionary *face_json = [NSJSONSerialization JSONObjectWithData:resData options:NSJSONReadingMutableLeaves error:nil];
    NSDictionary* faceobj = [((NSDictionary*)[((NSArray*)[face_json objectForKey:@"faces"]) objectAtIndex:0]) objectForKey:@"landmark"];
    
    // 脸
    std::vector<gpu_point_t>    faces;
    NSArray* facei = @[@"contour_left1", @"contour_left2", @"contour_left3", @"contour_left4", @"contour_left5", @"contour_left6", @"contour_left7", @"contour_left8", @"contour_left9", @"contour_chin", @"contour_right9", @"contour_right8", @"contour_right7", @"contour_right6", @"contour_right5", @"contour_right4", @"contour_right3", @"contour_right2", @"contour_right1"];
    for (NSString* pi in facei) {
        NSDictionary* f = [faceobj objectForKey:pi];
        gpu_point_t p = { [((NSNumber*)[f objectForKey:@"x"]) floatValue], [((NSNumber*)[f objectForKey:@"y"]) floatValue]};
        faces.push_back(p);
    }
    line->addPoints(&faces[0], (uint32_t)faces.size());
    
    // 鼻子
    std::vector<gpu_point_t>    noses;
    NSArray* nosei = @[@"nose_contour_left1", @"nose_contour_left2", @"nose_left", @"nose_contour_left3", @"nose_contour_lower_middle", @"nose_contour_right3", @"nose_right", @"nose_contour_right2", @"nose_contour_right1"];
    for (NSString* pi in nosei) {
        NSDictionary* f = [faceobj objectForKey:pi];
        gpu_point_t p = { [((NSNumber*)[f objectForKey:@"x"]) floatValue], [((NSNumber*)[f objectForKey:@"y"]) floatValue]};
        noses.push_back(p);
    }
    line->addPoints(&noses[0], (uint32_t)noses.size());
    
    // 嘴巴
    std::vector<gpu_point_t>    mouth0;
    NSArray* mouth0i = @[@"mouth_left_corner", @"mouth_upper_lip_left_contour2", @"mouth_upper_lip_left_contour1", @"mouth_upper_lip_top", @"mouth_upper_lip_right_contour2", @"mouth_right_corner"];
    for (NSString* pi in mouth0i) {
        NSDictionary* f = [faceobj objectForKey:pi];
        gpu_point_t p = { [((NSNumber*)[f objectForKey:@"x"]) floatValue], [((NSNumber*)[f objectForKey:@"y"]) floatValue]};
        mouth0.push_back(p);
    }
    line->addPoints(&mouth0[0], (uint32_t)mouth0.size());
    
    std::vector<gpu_point_t>    mouth1;
    NSArray* mouth1i = @[@"mouth_left_corner", @"mouth_upper_lip_left_contour3", @"mouth_lower_lip_top", @"mouth_upper_lip_right_contour3", @"mouth_right_corner"];
    for (NSString* pi in mouth1i) {
        NSDictionary* f = [faceobj objectForKey:pi];
        gpu_point_t p = { [((NSNumber*)[f objectForKey:@"x"]) floatValue], [((NSNumber*)[f objectForKey:@"y"]) floatValue]};
        mouth1.push_back(p);
    }
    line->addPoints(&mouth1[0], (uint32_t)mouth1.size());
    
    std::vector<gpu_point_t>    mouth2;
    NSArray* mouth2i = @[@"mouth_left_corner", @"mouth_lower_lip_left_contour1", @"mouth_lower_lip_right_contour1", @"mouth_right_corner"];
    for (NSString* pi in mouth2i) {
        NSDictionary* f = [faceobj objectForKey:pi];
        gpu_point_t p = { [((NSNumber*)[f objectForKey:@"x"]) floatValue], [((NSNumber*)[f objectForKey:@"y"]) floatValue]};
        mouth2.push_back(p);
    }
    line->addPoints(&mouth2[0], (uint32_t)mouth2.size());
    
    std::vector<gpu_point_t>    mouth3;
    NSArray* mouth3i = @[@"mouth_left_corner", @"mouth_lower_lip_left_contour2", @"mouth_lower_lip_left_contour3", @"mouth_lower_lip_bottom", @"mouth_lower_lip_right_contour3", @"mouth_lower_lip_right_contour2", @"mouth_right_corner"];
    for (NSString* pi in mouth3i) {
        NSDictionary* f = [faceobj objectForKey:pi];
        gpu_point_t p = { [((NSNumber*)[f objectForKey:@"x"]) floatValue], [((NSNumber*)[f objectForKey:@"y"]) floatValue]};
        mouth3.push_back(p);
    }
    line->addPoints(&mouth3[0], (uint32_t)mouth3.size());
    
    // 左眼
    std::vector<gpu_point_t>    leye0;
    NSArray* leye0i = @[@"left_eye_left_corner", @"left_eye_upper_left_quarter", @"left_eye_top", @"left_eye_upper_right_quarter", @"left_eye_right_corner"];
    for (NSString* pi in leye0i) {
        NSDictionary* f = [faceobj objectForKey:pi];
        gpu_point_t p = { [((NSNumber*)[f objectForKey:@"x"]) floatValue], [((NSNumber*)[f objectForKey:@"y"]) floatValue]};
        leye0.push_back(p);
    }
    line->addPoints(&leye0[0], (uint32_t)leye0.size());
    
    std::vector<gpu_point_t>    leye1;
    NSArray* leye1i = @[@"left_eye_left_corner", @"left_eye_lower_left_quarter", @"left_eye_bottom", @"left_eye_lower_right_quarter", @"left_eye_right_corner"];
    for (NSString* pi in leye1i) {
        NSDictionary* f = [faceobj objectForKey:pi];
        gpu_point_t p = { [((NSNumber*)[f objectForKey:@"x"]) floatValue], [((NSNumber*)[f objectForKey:@"y"]) floatValue]};
        leye1.push_back(p);
    }
    line->addPoints(&leye1[0], (uint32_t)leye1.size());
    
    // 右眼
    std::vector<gpu_point_t>    points;
    points.clear();
    NSArray* reye0i = @[@"right_eye_left_corner", @"right_eye_upper_left_quarter", @"right_eye_top", @"right_eye_upper_right_quarter", @"right_eye_right_corner"];
    for (NSString* pi in reye0i) {
        NSDictionary* f = [faceobj objectForKey:pi];
        gpu_point_t p = { [((NSNumber*)[f objectForKey:@"x"]) floatValue], [((NSNumber*)[f objectForKey:@"y"]) floatValue]};
        points.push_back(p);
    }
    line->addPoints(&points[0], (uint32_t)points.size());
    
    points.clear();
    NSArray* reye1i = @[@"right_eye_left_corner", @"right_eye_lower_left_quarter", @"right_eye_bottom", @"right_eye_lower_right_quarter", @"right_eye_right_corner"];
    for (NSString* pi in reye1i) {
        NSDictionary* f = [faceobj objectForKey:pi];
        gpu_point_t p = { [((NSNumber*)[f objectForKey:@"x"]) floatValue], [((NSNumber*)[f objectForKey:@"y"]) floatValue]};
        points.push_back(p);
    }
    line->addPoints(&points[0], (uint32_t)points.size());
}

-(void)piantSt{
    float face_points[] = {
        89.5, 627.6,86.1, 665.6,84.1, 703.8,84.5, 742.2,86.9, 780.4,90.9, 818.4,96.5, 856.2,104.4, 893.6,115.6, 930.1,130.7, 965.1,149.3, 998.3,171.0, 1029.4,195.7, 1058.1,223.6, 1083.3,255.9, 1102.4,291.9, 1113.9,329.6, 1118.6,367.6, 1117.4,404.9, 1109.7,439.6, 1094.2,470.8, 1072.5,499.0, 1047.2,524.5, 1019.2,547.2, 988.7,566.2, 955.8,581.3, 920.9,593.1, 884.6,602.5, 847.6,610.6, 810.3,617.3, 772.8,622.0, 735.0,624.6, 697.0,626.0, 658.9,120.5, 552.7,158.7, 515.2,206.5, 511.9,254.4, 523.1,300.0, 542.2,411.9, 548.8,459.8, 534.8,509.8, 529.1,558.5, 537.3,594.4, 579.1,352.2, 619.4,349.7, 663.4,347.0, 707.4,344.3, 751.8,270.9, 807.1,306.7, 811.8,342.3, 817.3,378.7, 815.3,414.7, 815.1,163.7, 621.4,189.7, 612.6,246.9, 613.5,271.0, 629.4,243.8, 633.6,189.1, 631.8,434.2, 639.1,459.5, 625.4,517.1, 630.9,542.8, 643.0,515.9, 650.4,460.9, 646.1,157.9, 550.3,203.9, 550.1,251.0, 557.9,296.6, 569.3,412.8, 576.3,459.7, 569.8,508.2, 567.1,555.6, 572.6,217.9, 610.5,216.3, 635.7,217.2, 623.4,488.8, 625.4,488.1, 651.1,488.5, 638.8,311.6, 624.3,393.3, 629.2,278.2, 734.3,414.7, 741.6,252.6, 777.5,436.0, 787.6,228.7, 926.4,262.2, 898.2,298.0, 877.8,336.7, 886.1,376.9, 882.6,410.0, 907.2,441.1, 939.1,409.4, 963.5,371.7, 978.2,331.3, 980.0,291.4, 973.4,256.5, 954.5,244.3, 925.6,289.7, 917.7,335.1, 922.1,381.2, 923.5,425.8, 936.4,380.5, 930.3,334.7, 929.0,289.7, 925.4,217.2, 623.4,488.5, 638.8
    };
    gpu_point_t points[106];
    for (int i=0; i<106; i++) {
        points[i].x = face_points[i*2];
        points[i].y = face_points[i*2+1];
    }
    
    line->addPoints(points, 33);
    // 嘴巴
    std::vector<gpu_point_t>    mouths;
    int mouth_index0[] = {84,85,86,87,88,89,90};
    for (int i=0; i<sizeof(mouth_index0)/sizeof(int); i++) {
        mouths.push_back(points[mouth_index0[i]]);
    }
    line->addPoints(&mouths[0], (uint32_t)mouths.size());
    
    mouths.clear();
    int mouth_index1[] = {90,91,92,93,94,95,84};
    for (int i=0; i<sizeof(mouth_index1)/sizeof(int); i++) {
        mouths.push_back(points[mouth_index1[i]]);
    }
    line->addPoints(&mouths[0], (uint32_t)mouths.size());
    
    mouths.clear();
    int mouth_index2[] = {84,96,97,99,100,90};
    for (int i=0; i<sizeof(mouth_index2)/sizeof(int); i++) {
        mouths.push_back(points[mouth_index2[i]]);
    }
    line->addPoints(&mouths[0], (uint32_t)mouths.size());
    
    mouths.clear();
    int mouth_index3[] = {100, 101, 102, 103,96};
    for (int i=0; i<sizeof(mouth_index3)/sizeof(int); i++) {
        mouths.push_back(points[mouth_index3[i]]);
    }
    line->addPoints(&mouths[0], (uint32_t)mouths.size());
    
    // 鼻子
    std::vector<gpu_point_t>    noses;
    int nose_index[] = {80, 82, 47, 48, 49, 50, 51, 83, 81};
    for (int i=0; i<sizeof(nose_index)/sizeof(int); i++) {
        noses.push_back(points[nose_index[i]]);
    }
    line->addPoints(&noses[0], (uint32_t)noses.size());
    
    // 左眼
    std::vector<gpu_point_t>    left_eye_tops;
    int lefteye_top[] = {52, 53, 72, 54, 55};
    for (int i=0; i<sizeof(lefteye_top)/sizeof(int); i++) {
        left_eye_tops.push_back(points[lefteye_top[i]]);
    }
    line->addPoints(&left_eye_tops[0], (uint32_t)left_eye_tops.size());
    std::vector<gpu_point_t>    left_eye_btms;
    int lefteye_btm[] = {55, 56, 73, 57, 52};
    for (int i=0; i<sizeof(lefteye_btm)/sizeof(int); i++) {
        left_eye_btms.push_back(points[lefteye_btm[i]]);
    }
    line->addPoints(&left_eye_btms[0], (uint32_t)left_eye_btms.size());
    
    // 右眼
    std::vector<gpu_point_t>    right_eye_tops;
    int righteye_top[] = {58, 59, 75, 60, 61};
    for (int i=0; i<sizeof(righteye_top)/sizeof(int); i++) {
        right_eye_tops.push_back(points[righteye_top[i]]);
    }
    line->addPoints(&right_eye_tops[0], (uint32_t)right_eye_tops.size());
    std::vector<gpu_point_t>    right_eye_btms;
    int righteye_btm[] = {58, 63, 76, 62, 61};
    for (int i=0; i<sizeof(righteye_btm)/sizeof(int); i++) {
        right_eye_btms.push_back(points[righteye_btm[i]]);
    }
    line->addPoints(&right_eye_btms[0], (uint32_t)right_eye_btms.size());
}

-(void)newFrame{
    line->newFrame();
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}


@end

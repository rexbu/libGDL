/**
 * file :	GPUIOSView.h
 * author :	Rex
 * email : rex@labjk.com
 * create :	2016-07-08 16:08
 * func :
 * history:
 */

#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#include "GPUIOSBuffer.h"
#include "GPUInput.h"

//typedef enum {
//    GPUFillModeStretch,                       // Stretch to fill the full view, which may distort the image outside of its normal aspect ratio
//    GPUFillModePreserveAspectRatio,           // Maintains the aspect ratio of the source image, adding bars of the specified background color
//    GPUFillModePreserveAspectRatioAndFill     // Maintains the aspect ratio of the source image, zooming in on its center to fill the view
//} GPUImageFillModeType; 

/**
 UIView subclass to use as an endpoint for displaying GPUImage outputs
 */
@interface GPUUIView : UIView
@property(assign, nonatomic) gpu_fill_mode_t fillMode;
@property(nonatomic, assign)gpu_rotation_t rotation;
@property(readonly, nonatomic) CGSize sizeInPixels;

@property(nonatomic) BOOL enabled;
- (void)newFrame;
-(void)setInputFrameBuffer:(GPUIOSFrameBuffer*)frameBuffer;

@end

class GPUIOSView:public GPUInput{
public:
    GPUIOSView(CGRect rect);
    ~GPUIOSView(){
        if (m_view!=nil) {
            m_view = nil;
        }
    }
    
    GPUUIView* uiview(){
        return m_view;
    }
    
    void newFrame();
    void setInputFrameBuffer(GPUFrameBuffer* buffer, int location=0);
    
    virtual void setOutputRotation(gpu_rotation_t rotation);
    
protected:
    GPUUIView*  m_view;
};

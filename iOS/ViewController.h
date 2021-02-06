//
//  ViewController.h
//  iOS
//
//  Created by Никита Исаенко on 13.01.2021.
//

#import <UIKit/UIKit.h>
#import <MetalKit/MetalKit.h>

@interface ViewController : UIViewController
@end

/** The Metal-compatibile view for the demo Storyboard. */
@interface View : MTKView
//@property(nonatomic, strong) NSDictionary<NSInteger, UITouch*>* touchList;
@end

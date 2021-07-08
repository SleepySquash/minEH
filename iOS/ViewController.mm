//
//  ViewController.m
//  iOS
//
//  Created by Никита Исаенко on 13.01.2021.
//

#import "ViewController.h"
#include "../minEH/Support/ApplicationState.hpp"
#include "../minEH/Window/Window.hpp"
#include "main.h"

@interface ViewController ()

@end

@implementation ViewController

CADisplayLink* _displayLink;

- (void)run {
    _main(0, NULL);
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    self.view.multipleTouchEnabled = YES;
    
    _applicationState.viewController = (__bridge void*)self;
    _applicationState.view = (__bridge void*)self.view;
    
    // _displayLink = [CADisplayLink displayLinkWithTarget: self selector: @selector(run)];
    // [_displayLink addToRunLoop: NSRunLoop.currentRunLoop forMode: NSDefaultRunLoopMode];
    // [self performSelector:@selector(run) withObject:nil afterDelay:0.0];
    // [self performSelectorInBackground:@selector(run) withObject:nil];
    _applicationState.thread = (void*)CFBridgingRetain([[NSThread alloc] initWithTarget:self selector:@selector(run) object:nil]);
    [(__bridge NSThread*)_applicationState.thread start];
}

- (void)viewWillTransitionToSize:(CGSize)size
       withTransitionCoordinator:(id<UIViewControllerTransitionCoordinator>)coordinator
{
    [super viewWillTransitionToSize:size withTransitionCoordinator:coordinator];
    
    if (_applicationState._window)
    {
        float scale = [[UIScreen mainScreen] scale];
        ((mh::Window*)_applicationState._window)->frame.width  = static_cast<unsigned int>(size.width );
        ((mh::Window*)_applicationState._window)->frame.height = static_cast<unsigned int>(size.height);
        ((mh::Window*)_applicationState._window)->width  = static_cast<unsigned int>(scale * size.width );
        ((mh::Window*)_applicationState._window)->height = static_cast<unsigned int>(scale * size.height);
        
        mh::Event event;
        event.type = mh::Event::Type::Resized;
        ((mh::Window*)_applicationState._window)->events.push(event);
    }
}

@end

@implementation View

/** Returns a Metal-compatible layer. */
+(Class) layerClass { return [CAMetalLayer class]; }

- (void)touchesBegan:(NSSet<UITouch *> *)touches
           withEvent:(UIEvent *)event {
    [super touchesBegan:touches withEvent:event];
    
    // NSUInteger touchCount = [touches count];
    // NSUInteger tapCount = [[touches anyObject] tapCount];
    
    NSUInteger finger = 0;
    for(UITouch* touch in touches) {
        // [touchList setValue:squiggle forKey:[touch hash]];
        
        CGPoint point = [touch locationInView:self];
        NSLog(@"%f %f", point.x, point.y);
        
        float scale = [[UIScreen mainScreen] scale];
        
        mh::Event event;
        event.type = mh::Event::Type::TouchBegan;
        event.data.touch.finger = (unsigned int)finger;
        event.data.touch.x = point.x * scale;
        event.data.touch.y = point.y * scale;
        ((mh::Window*)_applicationState._window)->events.push(event);
        
        finger++;
    }
}

- (void)touchesMoved:(NSSet<UITouch *> *)touches
           withEvent:(UIEvent *)event {
    [super touchesMoved:touches withEvent:event];
    
    NSUInteger finger = 0;
    for(UITouch* touch in touches) {
        // NSObject *valueFromDictionary = [myTouches valueForKey:[touch hash]];
        // [touchList removeObjectForKey:key];
        
        CGPoint point = [touch locationInView:self];
        float scale = [[UIScreen mainScreen] scale];
        
        mh::Event event;
        event.type = mh::Event::Type::TouchMoved;
        event.data.touch.finger = (unsigned int)finger;
        event.data.touch.x = point.x * scale;
        event.data.touch.y = point.y * scale;
        ((mh::Window*)_applicationState._window)->events.push(event);
        
        finger++;
    }
}

- (void)touchesEnded:(NSSet<UITouch *> *)touches
           withEvent:(UIEvent *)event {
    [super touchesEnded:touches withEvent:event];
    
    NSUInteger finger = 0;
    for(UITouch* touch in touches) {
        CGPoint point = [touch locationInView:self];
        float scale = [[UIScreen mainScreen] scale];
        
        mh::Event event;
        event.type = mh::Event::Type::TouchEnded;
        event.data.touch.finger = (unsigned int)finger;
        event.data.touch.x = point.x * scale;
        event.data.touch.y = point.y * scale;
        ((mh::Window*)_applicationState._window)->events.push(event);
        
        finger++;
    }
}

- (void)touchesCancelled:(NSSet<UITouch *> *)touches
               withEvent:(UIEvent *)event {
    [super touchesCancelled:touches withEvent:event];
    
    NSUInteger finger = 0;
    for(UITouch* touch in touches) {
        CGPoint point = [touch locationInView:self];
        float scale = [[UIScreen mainScreen] scale];
        
        mh::Event event;
        event.type = mh::Event::Type::TouchEnded;
        event.data.touch.finger = (unsigned int)finger;
        event.data.touch.x = point.x * scale;
        event.data.touch.y = point.y * scale;
        ((mh::Window*)_applicationState._window)->events.push(event);
        
        finger++;
    }
}

@end

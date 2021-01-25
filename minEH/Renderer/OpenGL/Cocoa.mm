//
//  Cocoa.m
//  macOS
//
//  Created by Никита Исаенко on 15.01.2021.
//

#include "../../Support/Config.hpp"
#ifdef MINEH_OPENGL

#define GL_SILENCE_DEPRECATION
#import <Cocoa/Cocoa.h>

#include "../Context.hpp"
#include "../../Support/ApplicationState.hpp"

@interface _NSWindow : NSWindow
@end

@interface _NSOpenGLView : NSOpenGLView
@end
@implementation _NSOpenGLView
- (void)keyDown:(NSEvent *)event { }
@end

namespace mh {
namespace GL
{
    Context::Context() { }
    Context::Context(Window* window) { create(window); }
    void Context::create(Window* window)
    {
        type = Renderer::Type::GL;
        
        this->window = window;
        NSOpenGLPixelFormatAttribute pixelFormatAttributes[] =
        {
            NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
            NSOpenGLPFAColorSize    , 24                           ,
            NSOpenGLPFAAlphaSize    , 8                            ,
            NSOpenGLPFADoubleBuffer ,
            NSOpenGLPFAAccelerated  ,
            0
        };
        NSOpenGLPixelFormat *pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:pixelFormatAttributes];
    
        NSRect rect = NSMakeRect(window->x, window->y, window->width, window->height);
        view = window->view = (void*)CFBridgingRetain([[_NSOpenGLView alloc] initWithFrame:rect pixelFormat:pixelFormat]);
        [(__bridge _NSOpenGLView*)window->view setHidden: NO];
        [(__bridge _NSOpenGLView*)window->view setNeedsDisplay: YES];
        [(__bridge _NSWindow*)window->window setContentView:(__bridge _NSOpenGLView*)window->view];
        [(__bridge _NSWindow*)window->window makeKeyAndOrderFront: nil];
        
        const NSRect contentRect = [(__bridge _NSOpenGLView*)view frame];
        const NSRect fbRect = [(__bridge _NSOpenGLView*)view convertRectToBacking: contentRect];
        window->frame.width = static_cast<unsigned int>(fbRect.size.width);
        window->frame.height = static_cast<unsigned int>(fbRect.size.height);
        
        setContext();
        init();
    }
    Context::~Context()
    {
        
    }
    
    void Context::setContext()
    {
        [[(__bridge _NSOpenGLView*)view openGLContext] makeCurrentContext];
    }
    
    void Context::endDraw(const uint32_t&)
    {
        [[(__bridge _NSOpenGLView*)view openGLContext] flushBuffer];
        [(__bridge _NSOpenGLView*)view setNeedsDisplay: YES];
    }
}
}

#endif

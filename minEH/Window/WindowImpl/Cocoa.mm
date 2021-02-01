//
//  Cocoa.m
//  macOS
//
//  Created by Никита Исаенко on 14.01.2021.
//

#import <Cocoa/Cocoa.h>
#import <CoreVideo/CVDisplayLink.h>
#import <QuartzCore/CAMetalLayer.h>

#include <iostream>

#include "Cocoa.h"
#include "../../Support/ApplicationState.hpp"

@implementation _NSWindow
@end

@implementation _NSApplicationDelegate
- (instancetype)initWithWindow:(mh::Window*)initWindow
{
    self = [super init];
    if (self != nil) window = initWindow;
    return self;
}

- (void)windowDidResize:(NSNotification *)notification
{
    @autoreleasepool
    {
        const NSRect contentRect = [(__bridge NSView*)window->view frame];
        const NSRect fbRect = [(__bridge NSView*)window->view convertRectToBacking: contentRect];
        window->frame.width = static_cast<unsigned int>(fbRect.size.width);
        window->frame.height = static_cast<unsigned int>(fbRect.size.height);
        
        const CGSize winRect = ((__bridge NSWindow*)window->window).frame.size;
        window->width = static_cast<unsigned int>(winRect.width);
        window->height = static_cast<unsigned int>(winRect.height);
    
        if (window->mResized)
        {
            mh::Event event;
            event.type = mh::Event::Type::Resized;
            window->events.push(event);
            
            [(__bridge NSView*)window->view setNeedsDisplay: YES];
            window->mResized = false;
        }
    }
}

- (BOOL)windowShouldClose:(id)sender
{
    mh::Event event;
    event.type = mh::Event::Type::Closed;
    window->events.push(event);
    return NO;
}
@end

@implementation _NSView
- (BOOL)acceptsFirstResponder
{
    return YES;
}
- (BOOL)isOpaque
{
    return YES;
}

/** Indicates that the view wants to draw using the backing layer instead of using drawRect:.  */
-(BOOL) wantsUpdateLayer { return YES; }
/** Returns a Metal-compatible layer. */
+(Class) layerClass { return [CAMetalLayer class]; }
/** If the wantsLayer property is set to YES, this method will be invoked to return a layer instance. */
-(CALayer*) makeBackingLayer {
    @autoreleasepool
    {
        CALayer* layer = [self.class.layerClass layer];
        CGSize viewScale = [self convertSizeToBacking: CGSizeMake(1.0, 1.0)];
        layer.contentsScale = MIN(viewScale.width, viewScale.height);
        return layer;
    }
}
@end

CVReturn displayCallback(CVDisplayLinkRef displayLink, const CVTimeStamp *inNow, const CVTimeStamp *inOutputTime, CVOptionFlags flagsIn, CVOptionFlags *flagsOut, void *displayLinkContext)
{
    @autoreleasepool
    {
        // mh::Window *window = (mh::Window*)displayLinkContext;
        // window->mayRender = true;
        DisplayData *data = (DisplayData*)displayLinkContext;
        
        data->window->mayRender = true;
        if (data->callback) data->callback(data->data);
        
        return kCVReturnSuccess;
    }
}

namespace mh
{
    Window::Window() { }
    Window::~Window() { }

    void Window::setCallback(void (*fun)(void*), void* data)
    {
        displayData.window = this;
        displayData.callback = fun;
        displayData.data = data;
        
        @autoreleasepool
        {
            CGDirectDisplayID displayID = CGMainDisplayID();
            CVDisplayLinkRef displayLink;
            CVReturn error = CVDisplayLinkCreateWithCGDisplay(displayID, &displayLink);
            if (kCVReturnSuccess == error)
            {
                CVDisplayLinkSetOutputCallback(displayLink, displayCallback, &displayData);
                CVDisplayLinkStart(displayLink);
            }
            else
            {
                NSLog(@"Display Link created with error: %d", error);
                displayLink = NULL;
            }
        }
    }
    void Window::open()
    {
        @autoreleasepool
        {
            NSRect rect = NSMakeRect(x, y, width, height);
            NSWindowStyleMask styleMask = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable | NSWindowStyleMaskMiniaturizable;
            
            // Setup NSWindow
            window = (void*)CFBridgingRetain([[_NSWindow alloc]
                                              initWithContentRect: rect
                                              styleMask: styleMask
                                              backing: NSBackingStoreBuffered
                                              defer: NO]);

            delegate = (void*)CFBridgingRetain( [[_NSApplicationDelegate alloc] initWithWindow: this] );
            
            setTitle(title);
            [(__bridge _NSWindow*)window setDelegate: (__bridge id<NSWindowDelegate> _Nullable)delegate];
            [(__bridge _NSWindow*)window setAcceptsMouseMovedEvents: YES];
            [(__bridge _NSWindow*)window center];
            if (fullscreen) [(__bridge _NSWindow*)window zoom: nil];
            
            // Setup NSView since we don't know whether user will be using OpenGL or not (cuz instead we'd create NSOpenGLView)
            view = (void*)CFBridgingRetain( [[_NSView alloc] initWithFrame:rect] );
            [(__bridge _NSView*)view setHidden: NO];
            [(__bridge _NSView*)view setNeedsDisplay: YES];
            [(__bridge _NSWindow*)window setContentView: (__bridge _NSView*)view];
            
            ((__bridge _NSView*)view).wantsLayer = YES;
            caMetalLayer = (__bridge void*)(((__bridge _NSView*)view).layer);
            [(__bridge CAMetalLayer*)caMetalLayer setDisplaySyncEnabled:NO];
            
            const NSRect contentRect = [(__bridge _NSView*)view frame];
            const NSRect fbRect = [(__bridge _NSView*)view convertRectToBacking: contentRect];
            frame.width = static_cast<unsigned int>(fbRect.size.width);
            frame.height = static_cast<unsigned int>(fbRect.size.height);
            
            const CGSize winRect = ((__bridge NSWindow*)window).frame.size;
            width = static_cast<unsigned int>(winRect.width);
            height = static_cast<unsigned int>(winRect.height);
        }
        
        focus();
        
#ifndef MINEH_WINDOW_NO_FIRST_RESIZE
        mh::Event event;
        event.type = mh::Event::Type::Resized;
        events.push(event);
        mResized = false;
#endif
        
        isOpen = true;
    }

    void Window::close()
    {
        @autoreleasepool
        {
            [(__bridge _NSWindow*)window close];
        }
        isOpen = false;
        window = nullptr;
    }

    void Window::focus()
    {
        @autoreleasepool
        {
            [NSApp activateIgnoringOtherApps: YES];
            [(__bridge _NSWindow*)window makeKeyAndOrderFront: nil];
        }
    }
    
    void Window::setTitle(const std::string& title)
    {
        @autoreleasepool
        {
            // TODO: it leaks memory!
            NSString *myNSString = [NSString stringWithUTF8String: title.c_str()];
            ((__bridge _NSWindow*)window).title = myNSString;
        }
    }
    
    void Window::setCursorMode(const Mouse::CursorMode& mode)
    {
        
    }

    /*
    Four            21
    Five            23
    Six             22
    Seven           26
    Eight           28
    Nine            25
    A               0
    B               11
    C               8
    D               2
    E               14
    F               3
    G               5
    H               4
    I               34
    J               38
    K               40
    L               37
    M               46
    N               45
    O               31
    P               35
    Q               12
    R               15
    S               1
    T               17
    U               32
    V               9
    W               13
    X               7
    Y               16
    Z               6
    SectionSign     10
    Grave           50
    Minus           27
    Equal           24
    LeftBracket     33
    RightBracket    30
    Semicolon       41
    Quote           39
    Comma           43
    Period          47
    Slash           44
    Backslash       42
    Keypad0 0       82
    Keypad1 1       83
    Keypad2 2       84
    Keypad3 3       85
    Keypad4 4       86
    Keypad5 5       87
    Keypad6 6       88
    Keypad7 7       89
    Keypad8 8       91
    Keypad9 9       92
    KeypadDecimal   65
    KeypadMultiply  67
    KeypadPlus      69
    KeypadDivide    75
    KeypadMinus     78
    KeypadEquals    81
    KeypadClear     71
    KeypadEnter     76
    Space           49
    Return          36
    Tab             48
    Delete          51
    ForwardDelete   117
    Linefeed        52
    Escape          53
    Command         55
    Shift           56
    CapsLock        57
    Option          58
    Control         59
    RightShift      60
    RightOption     61
    RightControl    62
    Function        63
    F1              122
    F2              120
    F3              99
    F4              118
    F5              96
    F6              97
    F7              98
    F8              100
    F9              101
    F10             109
    F11             103
    F12             111
    F13             105
    F14             107
    F15             113
    F16             106
    F17             64
    F18             79
    F19             80
    F20             90
    VolumeUp        72
    VolumeDown      73
    Mute            74
    Help/Insert     114
    Home            115
    End             119
    PageUp          116
    PageDown        121
    */
     
    Keyboard::Key keyboardGetKey(unsigned short keyCode)
    {
        switch (keyCode)
        {
            default: return Keyboard::Key::Unknown; break;
            case 29: return Keyboard::Key::Num0; break;
            case 18: return Keyboard::Key::Num1; break;
            case 19: return Keyboard::Key::Num2; break;
            case 20: return Keyboard::Key::Num3; break;
            case 56: return Keyboard::Key::LShift; break;
            case 12: return Keyboard::Key::Q; break;
            case 13: return Keyboard::Key::W; break;
            case 14: return Keyboard::Key::E; break;
            case 15: return Keyboard::Key::R; break;
            case 17: return Keyboard::Key::T; break;
            case 16: return Keyboard::Key::Y; break;
            case 32: return Keyboard::Key::U; break;
            case 34: return Keyboard::Key::I; break;
            case 31: return Keyboard::Key::O; break;
            case 35: return Keyboard::Key::P; break;
            case 0: return Keyboard::Key::A; break;
            case 1: return Keyboard::Key::S; break;
            case 2: return Keyboard::Key::D; break;
            case 3: return Keyboard::Key::F; break;
            case 5: return Keyboard::Key::G; break;
            case 4: return Keyboard::Key::H; break;
            case 38: return Keyboard::Key::J; break;
            case 40: return Keyboard::Key::K; break;
            case 37: return Keyboard::Key::L; break;
            case 6: return Keyboard::Key::Z; break;
            case 7: return Keyboard::Key::X; break;
            case 8: return Keyboard::Key::C; break;
            case 9: return Keyboard::Key::V; break;
            case 11: return Keyboard::Key::B; break;
            case 45: return Keyboard::Key::N; break;
            case 46: return Keyboard::Key::M; break;
            case 123: return Keyboard::Key::Left; break;
            case 124: return Keyboard::Key::Right; break;
            case 125: return Keyboard::Key::Down; break;
            case 126: return Keyboard::Key::Up; break;
            case 53: return Keyboard::Key::Escape; break;
        }
    }
    
    bool Window::pollEvent(Event& event)
    {
        @autoreleasepool
        {
            NSApplication* application = (__bridge NSApplication*)applicationState.application;
            NSEvent* e = nil;
            do
            {
                e = [application nextEventMatchingMask:NSEventMaskAny untilDate:nil inMode:NSDefaultRunLoopMode dequeue:YES];
                switch (e.type)
                {
                    case NSEventTypeSystemDefined: break;
                    case NSEventTypeKeyDown:
                    {
                        mh::Event event;
                        event.type = mh::Event::Type::KeyPressed;
                        event.data.key = keyboardGetKey(e.keyCode);
                        events.push(event);
                        e = nil;
                    } break;
                    case NSEventTypeKeyUp:
                    {
                        mh::Event event;
                        event.type = mh::Event::Type::keyReleased;
                        event.data.key = keyboardGetKey(e.keyCode);
                        events.push(event);
                    } break;
                    case NSEventTypeLeftMouseDown:
                    {
                        mh::Event event;
                        event.type = mh::Event::Type::MousePressed;
                        event.data.mouse.button.button = mh::Mouse::Button::Left;
                        events.push(event);
                    } break;
                    case NSEventTypeLeftMouseUp:
                    {
                        mh::Event event;
                        event.type = mh::Event::Type::MouseReleased;
                        event.data.mouse.button.button = mh::Mouse::Button::Left;
                        events.push(event);
                    } break;
                    case NSEventTypeRightMouseDown:
                    {
                        mh::Event event;
                        event.type = mh::Event::Type::MousePressed;
                        event.data.mouse.button.button = mh::Mouse::Button::Right;
                        events.push(event);
                    } break;
                    case NSEventTypeRightMouseUp:
                    {
                        mh::Event event;
                        event.type = mh::Event::Type::MouseReleased;
                        event.data.mouse.button.button = mh::Mouse::Button::Right;
                        events.push(event);
                    } break;
                    case NSEventTypeOtherMouseUp: break;
                    case NSEventTypeMouseMoved:
                    {
                        mh::Event event;
                        event.type = mh::Event::Type::MouseMove;
                        event.data.mouse.move.x = e.deltaX;
                        event.data.mouse.move.y = e.deltaY;
                        events.push(event);
                    } break;
                    case NSEventTypeScrollWheel:
                    {
                        mh::Event event;
                        event.type = mh::Event::Type::Scroll;
                        event.data.mouse.move.x = e.scrollingDeltaX;
                        event.data.mouse.move.y = e.scrollingDeltaY;
                        events.push(event);
                    } break;
                    default: break;
                }
                [application sendEvent:e];
            }
            while (e);
        }
        
        // [application updateWindows];
        
        if (events.empty()) return false;
        else
        {
            event = events.front();
            events.pop();
            return true;
        }
    }

    void Window::display()
    {
        @autoreleasepool
        {
            NSApplication* application = (__bridge NSApplication*)applicationState.application;
            [application updateWindows];
        }
        mayRender = shouldRender = false;
    }
} // namespace mh

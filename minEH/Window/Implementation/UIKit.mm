//
//  UIKit.m
//  iOS App
//
//  Created by Никита Исаенко on 06.02.2021.
//

#import <UIKit/UIKit.h>
#import <MetalKit/MetalKit.h>

#include <iostream>

#include "UIKit.h"
#include "../../Support/ApplicationState.hpp"

namespace mh
{
    Window::Window() { }
    Window::~Window() { }

    void Window::setCallback(void (*fun)(void*), void* data) { }
    void Window::open()
    {
        // UIApplication* app = (__bridge UIApplication*)applicationState.application;
        // scene = (void*)CFBridgingRetain([SceneDelegate alloc]);

        CGRect rect = [[UIScreen mainScreen] bounds];
        // CGRect nativeRect = [[UIScreen mainScreen] nativeBounds];
        float scale = [[UIScreen mainScreen] scale];
        
        width  = static_cast<unsigned int>(rect.size.width );
        height = static_cast<unsigned int>(rect.size.height);
        frame.width  = static_cast<unsigned int>(scale * rect.size.width );
        frame.height = static_cast<unsigned int>(scale * rect.size.height);

        
        _applicationState._window = this;
        MTKView* v = (__bridge MTKView*)_applicationState.view;
        caMetalLayer = (__bridge void*)(v.layer);
        [v setAutoResizeDrawable:YES];
        
        
        // Configure view
        /*view = (void*)CFBridgingRetain( [[_MTKView alloc] initWithFrame:rect] );
        _MTKView* v = (__bridge _MTKView*)view;
        caMetalLayer = (__bridge void*)(v.layer);
        [v resignFirstResponder];
        [v setNeedsDisplay];
        [v setHidden:NO];
        [v setOpaque:YES];
        [v setAutoResizeDrawable:YES];
        v.backgroundColor = [UIColor clearColor];
        v._window = this;
        
        
        viewController = (void*)CFBridgingRetain( [[_UIViewController alloc] init] );
        _UIViewController* vc = (__bridge _UIViewController*)viewController;
        [vc setView:v];
        // [vc setNeedsUpdateOfHomeIndicatorAutoHidden];
        // [vc setNeedsStatusBarAppearanceUpdate];
        
        // Configure window
        // window = (void*)CFBridgingRetain( [ [_UIWindow alloc] initWithWindowScene:(UIWindowScene*)[ [ [UIApplication sharedApplication] connectedScenes] anyObject] ] );
        // _UIWindow* w = (__bridge _UIWindow*)window;
        // window = (__bridge void*)( [ [ [UIApplication sharedApplication] windows] objectAtIndex:0] );
        // UIWindow* w = (__bridge UIWindow*)window;
        // window = (void*)CFBridgingRetain( [[_UIWindow alloc] initWithFrame:rect] );
        window = (__bridge void*)[ [ [UIApplication sharedApplication] keyWindow ] initWithFrame:rect ];
        _UIWindow* w = (__bridge _UIWindow*)window;
        [w setRootViewController:vc];
        [w setContentMode:UIViewContentModeScaleToFill];
        [w setBounds:rect];
        [w makeKeyAndVisible];
        w.backgroundColor = [UIColor blueColor];*/
        
#ifndef MINEH_WINDOW_NO_FIRST_RESIZE
        mh::Event event;
        event.type = mh::Event::Type::Resized;
        events.push(event);
#endif
        
        isOpen = true;
    }

    void Window::close()
    {
        window = nullptr;
        view = nullptr;
        viewController = nullptr;
        caMetalLayer = nullptr;
    }

    void Window::focus()
    {
        
    }
    
    void Window::setTitle(const std::string& title)
    {
        
    }
    
    void Window::setCursorMode(const Mouse::CursorMode& mode)
    {
        
    }
    
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
        
        mayRender = shouldRender = false;
    }
}

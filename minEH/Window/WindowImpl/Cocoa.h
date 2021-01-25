//
//  Cocoa.h
//  macOS
//
//  Created by Никита Исаенко on 16.01.2021.
//

#ifndef WindowImpl_Cocoa_h
#define WindowImpl_Cocoa_h

#include "../Window.hpp"


@interface _NSWindow : NSWindow
@end


@interface _NSApplicationDelegate : NSObject<NSApplicationDelegate>
{
    mh::Window* window;
}
- (instancetype)initWithWindow:(mh::Window*)initWindow;
@end


@interface _NSView : NSView
- (BOOL)    acceptsFirstResponder;
- (BOOL)    isOpaque;
@end


#endif /* WindowImpl_Cocoa_h */

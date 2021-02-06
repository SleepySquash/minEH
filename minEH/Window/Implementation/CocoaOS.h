//
//  CocoaOS.h
//  macOS
//
//  Created by Никита Исаенко on 14.01.2021.
//

#ifndef CocoaOS_h
#define CocoaOS_h

#import <Cocoa/Cocoa.h>

struct Window_OSSpecificData
{
    NSWindow* window;
    NSView* view;
    
    NSString* mTitle;
};

#endif /* CocoaOS_h */

//
//  main.m
//  macOS
//
//  Created by Никита Исаенко on 13.01.2021.
//

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

#include "../minEH/Support/ApplicationState.hpp"
#include "main.h"

// First we define an NSApplication so that our program is defined as an App in macOS (not just command line etc)
@interface _NSApplication : NSApplication
- (void) run;
@end

// We define our own run() method so we can call user's _main() function
@implementation _NSApplication
- (void)run
{
    [[NSNotificationCenter defaultCenter]
        postNotificationName:NSApplicationWillFinishLaunchingNotification
        object:NSApp];
    [[NSNotificationCenter defaultCenter]
        postNotificationName:NSApplicationDidFinishLaunchingNotification
        object:NSApp];
    
    _main(mh::applicationState.argc, mh::applicationState.argv);
}
@end

// OS-specific main()
int main(int argc, const char * argv[])
{
    @autoreleasepool
    {
        // Create an instance of our defined NSApplication
        NSApplication* applicationObject = [_NSApplication alloc];
        
        // We put the pointer to NSApplication in our State object so we can use it to create NSWindow
        /// xwin::init(argc, (const char**)argv, applicationObject);
        mh::applicationState.argc = argc;
        mh::applicationState.argv = argv;
        mh::applicationState.application = (__bridge void*)applicationObject;

        // Run our NSApplication with method run() on the main thread
        if ([applicationObject respondsToSelector:@selector(run)])
        {
            [applicationObject
                performSelectorOnMainThread:@selector(run)
                withObject:nil
                waitUntilDone:YES];
        }
    }
    
    return 0;
}

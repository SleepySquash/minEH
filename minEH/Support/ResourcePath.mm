//
//  ResourcePath.m
//  OpenGL App
//
//  Created by Никита Исаенко on 15.01.2021.
//

#include "ResourcePath.hpp"
#import <Foundation/Foundation.h>

namespace mh
{
#ifdef MINEH_IOS
    /*std::string resourcePath(void)
    {
        NSString *folder = [[NSBundle mainBundle] resourcePath];
    }*/
#endif
    
#if defined(MINEH_MACOS) || defined(MINEH_IOS)
    std::string resourcePath(void)
    {
        std::string rpath;
        NSBundle* bundle = [NSBundle mainBundle];
        
        if (bundle == nil) {
    #ifdef DEBUGr
            NSLog(@"bundle is nil... thus no resources path can be found.");
    #endif
        } else {
            NSString* path = [bundle resourcePath];
            rpath = [path UTF8String] + std::string("/");
        }
        
        return rpath;
    }
#endif
}

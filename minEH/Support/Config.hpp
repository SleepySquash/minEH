//
//  Config.hpp
//  OpenGL App
//
//  Created by Никита Исаенко on 14.01.2021.
//


#pragma mark -
#pragma mark You're free to comment out

#define MINEH_DEBUG

// #define MINEH_WINDOW_NO_FIRST_RESIZE

#define MINEH_OPENGL
#define MINEH_VULKAN
// #define MINEH_METAL
// #define MINEH_WEBGL
// #define MINEH_DIRECTX
// #define MINEH_DIRECTX12

#pragma mark -





















#pragma mark -
#pragma mark You shouldn't modify these, please

#if defined(__APPLE__)
    #include <TargetConditionals.h>
    #if defined(TARGET_OS_OSX)
        #define MINEH_MACOS
    #else
        #define MINEH_IOS
    #endif
#elif defined(__ANDROID__)
    #define MINEH_ANDROID
#elif defined(_WIN32)
    #define MINEH_WIN32
#elif defined(unix) || defined(__unix__) || defined(__unix)
    #define MINEH_UNIX
#endif

#ifdef MINEH_OPENGL
    #define GL_SILENCE_DEPRECATION
#endif

#pragma mark -

//
//  Config.hpp
//  OpenGL App
//
//  Created by Никита Исаенко on 14.01.2021.
//


#pragma mark -
#pragma mark You're free to comment out

// MINEH_DEBUG is a light debug mode for the application to not crash at some situations (e.g. when the sprite's texture is not loaded). You actually should leave this define even if you're at Release build, but if need extra speed and you are certain that everything is nice ("if's" won't be fired) than you can comment this one.
#define MINEH_DEBUG

// By default window.open() places a Resized event to the queue. Uncomment this define to change that behaviour.
// #define MINEH_WINDOW_NO_FIRST_RESIZE

// These defines just specify Renderer that you want to support. Those that your OS does not support will be undefined below.
#define MINEH_OPENGL
#define MINEH_VULKAN
// #define MINEH_METAL
// #define MINEH_WEBGL
// #define MINEH_DIRECTX
// #define MINEH_DIRECTX12
// #define MINEH_CPU

// Define if you want Vulkan's validation layers
// #define MINEH_VULKAN_VALIDATION

// This define means that MoltenVK (and not the Vulkan Loader for macOS which uses MoltenVK as well, but does not contain extended MoltenVK headers and functions) is used, so some optimization at a context creation time may be made.
// #define MINEH_MOLTENVK

// About validation layers on macOS: MoltenVK does not support them, but Vulkan Loader does.
// For them to work:
// 1. Link libvulkan.1.2.131.dylib or any other version.
// 2. Unlink libMoltenVk.dylib.
// 3. But don't delete libMoltenVk.dylib from Copy phase.
// 4. Finally, in the context set enableValidationLayers to true 'context.enableValidationLayers = true;' before 'context.create(&window);'

#pragma mark -





















#pragma mark -
#pragma mark You shouldn't modify these, please

// Below are just OS defines
#if defined(__APPLE__)
    #include <TargetConditionals.h>
    #if defined(TARGET_OS_IOS)
        #define MINEH_IOS
    #else
        #if defined(TARGET_OS_OSX) // defined on iOS as well
            #define MINEH_MACOS
        #endif
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

// Undefine Renderers that are not supported by the OS
#ifdef MINEH_OPENGL
    #if defined(MINEH_IOS)
        #undef MINEH_OPENGL
    #endif
#endif

#ifdef MINEH_METAL
    #if !defined(MINEH_MACOS) && !defined(MINEH_IOS)
        #undef MINEH_METAL
    #endif
#endif

#ifdef MINEH_DIRECTX
    #if !defined(MINEH_WIN32)
        #undef MINEH_DIRECTX
    #endif
#endif

#ifdef MINEH_DIRECTX12
    #if !defined(MINEH_WIN32)
        #undef MINEH_DIRECTX12
    #endif
#endif

#pragma mark -

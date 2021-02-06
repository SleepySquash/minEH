//
//  State.hpp
//  OpenGL App
//
//  Created by Никита Исаенко on 14.01.2021.
//

#pragma once

#include "Config.hpp"

struct _ApplicationState
{
    int argc;
    const char** argv;
    
#ifdef MINEH_MACOS
    void* application; // Pointer to the NSApplication so we can use it to create NSWindow
#endif
#ifdef MINEH_IOS
    void *application, *delegate, *view, *viewController, *window, *thread;
    void* _window;
#endif
};

extern struct _ApplicationState _applicationState;

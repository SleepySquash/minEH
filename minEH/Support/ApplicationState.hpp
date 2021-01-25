//
//  State.hpp
//  OpenGL App
//
//  Created by Никита Исаенко on 14.01.2021.
//

#pragma once

#include "Config.hpp"

namespace mh
{
    struct ApplicationState
    {
        int argc;
        const char** argv;
        
    #if defined(MINEH_MACOS) || defined(MINEH_IOS)
        void* application; // Pointer to the NSApplication so we can use it to create NSWindow
    #endif
    };

    extern struct ApplicationState applicationState;
}

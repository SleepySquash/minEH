//
//  ResourcePath.hpp
//  OpenGL App
//
//  Created by Никита Исаенко on 02.01.2021.
//

#pragma once

#include "Config.hpp"
#include <string>

namespace mh
{
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
   //define something for Windows (32-bit and 64-bit, this part is common)
   #ifdef _WIN64
      //define something for Windows (64-bit only)
   #else
      //define something for Windows (32-bit only)
   #endif
#elif __APPLE__
    #include <TargetConditionals.h>
    #if TARGET_IPHONE_SIMULATOR
        std::string resourcePath();
    #elif TARGET_OS_IPHONE
        std::string resourcePath();
    #elif TARGET_OS_MAC
        std::string resourcePath();
    #else
    #   error "Unknown Apple platform"
    #endif
#elif __linux__
    // linux
#elif __unix__ // all unices not caught above
    // Unix
#elif defined(_POSIX_VERSION)
    // POSIX
#else
#   error "Unknown compiler"
#endif
}

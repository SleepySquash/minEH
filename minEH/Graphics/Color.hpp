//
//  Color.hpp
//  OpenGL App
//
//  Created by Никита Исаенко on 17.01.2021.
//

#ifndef Color_hpp
#define Color_hpp

#include <stdio.h>

namespace mh
{
    struct Color
    {
        float r, g, b, a;
        Color(const float r = 0.f, const float g = 0.f, const float b = 0.f, const float a = 1.f);
        
        static const Color Black;
        static const Color White;
    };
}

#endif /* Color_hpp */

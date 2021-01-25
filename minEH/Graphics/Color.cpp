//
//  Color.cpp
//  OpenGL App
//
//  Created by Никита Исаенко on 17.01.2021.
//

#include "Color.hpp"

namespace mh
{
    Color::Color(const float r, const float g, const float b, const float a) : r(r), g(g), b(b), a(a) { }
    
    const Color Black = Color();
    const Color White = Color(1.f, 1.f, 1.f);
}

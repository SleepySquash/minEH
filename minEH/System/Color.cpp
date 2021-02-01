//
//  Color.cpp
//  OpenGL App
//
//  Created by Никита Исаенко on 17.01.2021.
//

#include "Color.hpp"

namespace mh
{
    glm::vec4 uColor(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) { return glm::vec4(255.f/r, 255.f/g, 255.f/b, 255.f/a); }
    glm::vec4 iColor(const int r, const int g, const int b, const int a) { return glm::vec4(255.f/r, 255.f/g, 255.f/b, 255.f/a); }
}

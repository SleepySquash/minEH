//
//  Color.hpp
//  OpenGL App
//
//  Created by Никита Исаенко on 17.01.2021.
//

#pragma once

#include <cstdint>
#include <glm/glm.hpp>

namespace mh
{
    glm::vec4 uColor(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a);
    glm::vec4 iColor(const int r, const int g, const int b, const int a);
        
    namespace Color
    {
        constexpr glm::vec4 Black   = glm::vec4(0.f, 0.f, 0.f, 1.f);
        constexpr glm::vec4 White   = glm::vec4(1.f, 1.f, 1.f, 1.f);
        constexpr glm::vec4 Red     = glm::vec4(1.f, 0.f, 0.f, 1.f);
        constexpr glm::vec4 Magenta = glm::vec4(1.f, 1.f, 1.f, 1.f);
        constexpr glm::vec4 Cyan    = glm::vec4(1.f, 1.f, 1.f, 1.f);
        constexpr glm::vec4 Yellow  = glm::vec4(1.f, 1.f, 1.f, 1.f);
    }
}

//
//  Camera.hpp
//  minEH
//
//  Created by Никита Исаенко on 24.01.2021.
//

#pragma once

#include <glm/glm.hpp>

namespace mh
{
    struct Camera
    {
        glm::vec3 position = glm::vec3(0.f, 0.f, 1.f),
                  direction = glm::vec3(0.f, 0.f, 1.f),
                  up = glm::vec3(0.f, 1.f, 0.f);
        float fov = glm::radians(60.f), near = 0.01f, far = 100.f;
        
        virtual ~Camera();
        
        virtual void create() = 0;
        virtual void destroy() = 0;
        virtual void resize() = 0; // recreate perspective matrix and do dirty()
        virtual void update() = 0; // recreate view matrix and do dirty()
        virtual void draw(const uint32_t& i) = 0; // if dirty then update buffers
        
        virtual void setPosition(const glm::vec3& pos);
        virtual void setFOV(const float& fov);
        virtual void dirty() = 0;
        
        virtual void setContext(void* context) = 0;
    };
}

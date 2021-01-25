//
//  Factory.cpp
//  minEH
//
//  Created by Никита Исаенко on 24.01.2021.
//

#include "Factory.hpp"

namespace mh
{
    namespace Graphics
    {
        GL::GLSprite* GetSprite(GL::Context* context) { return new GL::GLSprite(context); }
        Vk::VkSprite* GetSprite(Vk::Context* context) { return new Vk::VkSprite(context); }
        
        GL::GLCamera* GetCamera(GL::Context* context) { GL::GLCamera* camera = new GL::GLCamera(context); camera->create(); return camera; }
        Vk::VkCamera* GetCamera(Vk::Context* context) { Vk::VkCamera* camera = new Vk::VkCamera(context); camera->create(); return camera; }
        
        GL::GLMesh* GetMesh(GL::Context* context) { return new GL::GLMesh(context); }
        Vk::VkMesh* GetMesh(Vk::Context* context) { return new Vk::VkMesh(context); }
    }
}

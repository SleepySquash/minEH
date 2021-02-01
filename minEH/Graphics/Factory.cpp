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
        Sprite* GetSprite(Renderer* context)
        {
            switch (context->type)
            {
                default: return nullptr; break;
                case Renderer::Type::GL: return new GL::GLSprite((GL::Context*)context); break;
                case Renderer::Type::Vk: return new Vk::VkSprite((Vk::Context*)context); break;
            }
        }
        
        GL::GLCamera* GetCamera(GL::Context* context) { GL::GLCamera* camera = new GL::GLCamera(context); camera->create(); return camera; }
        Vk::VkCamera* GetCamera(Vk::Context* context) { Vk::VkCamera* camera = new Vk::VkCamera(context); camera->create(); return camera; }
        Camera* GetCamera(Renderer* context)
        {
            switch (context->type)
            {
                default: return nullptr; break;
                case Renderer::Type::GL: { GL::GLCamera* camera = new GL::GLCamera((GL::Context*)context); camera->create(); return camera; } break;
                case Renderer::Type::Vk: { Vk::VkCamera* camera = new Vk::VkCamera((Vk::Context*)context); camera->create(); return camera; } break;
            }
        }
        
        GL::GLMesh* GetMesh(GL::Context* context) { return new GL::GLMesh(context); }
        Vk::VkMesh* GetMesh(Vk::Context* context) { return new Vk::VkMesh(context); }
        Mesh* GetMesh(Renderer* context)
        {
            switch (context->type)
            {
                default: return nullptr; break;
                case Renderer::Type::GL: return new GL::GLMesh((GL::Context*)context); break;
                case Renderer::Type::Vk: return new Vk::VkMesh((Vk::Context*)context); break;
            }
        }
    }
}

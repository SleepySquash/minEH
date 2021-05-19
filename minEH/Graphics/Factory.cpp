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
#ifdef MINEH_OPENGL
        GL::GLSprite* GetSprite(GL::Context* context) { return new GL::GLSprite(context); }
        GL::GLFont* GetFont(GL::Context* context) { return new GL::GLFont(context); }
        GL::GLText* GetText(GL::Context* context) { return new GL::GLText(context); }
        GL::GLCamera* GetCamera(GL::Context* context) { GL::GLCamera* camera = new GL::GLCamera(context); camera->create(); return camera; }
        GL::GLMesh* GetMesh(GL::Context* context) { return new GL::GLMesh(context); }
#endif
#ifdef MINEH_VULKAN
        Vk::VkSprite* GetSprite(Vk::Context* context) { return new Vk::VkSprite(context); }
        // Vk::VkFont* GetFont(Vk::Context* context) { return new Vk::VkFont(context); }
        // Vk::VkText* GetText(Vk::Context* context) { return new Vk::VkText(context); }
        Vk::VkCamera* GetCamera(Vk::Context* context) { Vk::VkCamera* camera = new Vk::VkCamera(context); camera->create(); return camera; }
        Vk::VkMesh* GetMesh(Vk::Context* context) { return new Vk::VkMesh(context); }
#endif
        
        Sprite* GetSprite(Renderer* context)
        {
            switch (context->type)
            {
                default: return nullptr; break;
#ifdef MINEH_OPENGL
                case Renderer::Type::GL: return new GL::GLSprite((GL::Context*)context); break;
#endif
#ifdef MINEH_VULKAN
                case Renderer::Type::Vk: return new Vk::VkSprite((Vk::Context*)context); break;
#endif
            }
        }
        
        Font* GetFont(Renderer* context)
        {
            switch (context->type)
            {
                default: return nullptr; break;
#ifdef MINEH_OPENGL
                case Renderer::Type::GL: return new GL::GLFont((GL::Context*)context); break;
#endif
#ifdef MINEH_VULKAN
                // case Renderer::Type::Vk: return new Vk::VkSprite((Vk::Context*)context); break;
#endif
            }
        }
        
        Text* GetText(Renderer* context)
        {
            switch (context->type)
            {
                default: return nullptr; break;
#ifdef MINEH_OPENGL
                case Renderer::Type::GL: return new GL::GLText((GL::Context*)context); break;
#endif
#ifdef MINEH_VULKAN
                // case Renderer::Type::Vk: return new Vk::VkSprite((Vk::Context*)context); break;
#endif
            }
        }
        
        Camera* GetCamera(Renderer* context)
        {
            switch (context->type)
            {
                default: return nullptr; break;
#ifdef MINEH_OPENGL
                case Renderer::Type::GL: { GL::GLCamera* camera = new GL::GLCamera((GL::Context*)context); camera->create(); return camera; } break;
#endif
#ifdef MINEH_VULKAN
                case Renderer::Type::Vk: { Vk::VkCamera* camera = new Vk::VkCamera((Vk::Context*)context); camera->create(); return camera; } break;
#endif
            }
        }
        
        Mesh* GetMesh(Renderer* context)
        {
            switch (context->type)
            {
                default: return nullptr; break;
#ifdef MINEH_OPENGL
                case Renderer::Type::GL: return new GL::GLMesh((GL::Context*)context); break;
#endif
#ifdef MINEH_VULKAN
                case Renderer::Type::Vk: return new Vk::VkMesh((Vk::Context*)context); break;
#endif
            }
        }
    }
}

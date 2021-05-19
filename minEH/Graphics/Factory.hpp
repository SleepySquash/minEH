//
//  Factory.hpp
//  minEH
//
//  Created by Никита Исаенко on 24.01.2021.
//

#pragma once

#include "../Support/Config.hpp"
#include "../Renderer/Context.hpp"
#include "../Renderer/OpenGL/Sprite.hpp"
#include "../Renderer/OpenGL/Text.hpp"
#include "../Renderer/OpenGL/Camera.hpp"
#include "../Renderer/OpenGL/Mesh.hpp"
#include "../Renderer/Vulkan/Sprite.hpp"
#include "../Renderer/Vulkan/Camera.hpp"
#include "../Renderer/Vulkan/Mesh.hpp"
#include "Drawable.hpp"

namespace mh
{
    namespace Graphics
    {
#ifdef MINEN_OPENGL
        GL::GLSprite* GetSprite(GL::Context* context);
        GL::GLFont* GetFont(GL::Context* context);
        GL::GLText* GetText(GL::Context* context);
        GL::GLCamera* GetCamera(GL::Context* context);
        GL::GLMesh* GetMesh(GL::Context* context);
#endif
#ifdef MINEH_VULKAN
        Vk::VkSprite* GetSprite(Vk::Context* context);
        // Vk::VkFont* GetFont(Vk::Context* context);
        // Vk::VkText* GetText(Vk::Context* context);
        Vk::VkCamera* GetCamera(Vk::Context* context);
        Vk::VkMesh* GetMesh(Vk::Context* context);
#endif
        
        Sprite* GetSprite(Renderer* context);
        Font* GetFont(Renderer* context);
        Text* GetText(Renderer* context);
        Camera* GetCamera(Renderer* context);
        Mesh* GetMesh(Renderer* context);
    }
}

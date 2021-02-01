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
        GL::GLSprite* GetSprite(GL::Context* context);
        Vk::VkSprite* GetSprite(Vk::Context* context);
        Sprite* GetSprite(Renderer* context);
        
        GL::GLCamera* GetCamera(GL::Context* context);
        Vk::VkCamera* GetCamera(Vk::Context* context);
        Camera* GetCamera(Renderer* context);
        
        GL::GLMesh* GetMesh(GL::Context* context);
        Vk::VkMesh* GetMesh(Vk::Context* context);
        Mesh* GetMesh(Renderer* context);
    }
}

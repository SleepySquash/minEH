//
//  Mesh.hpp
//  minEH
//
//  Created by Никита Исаенко on 24.01.2021.
//

#pragma once

#include "../../Support/Config.hpp"

#ifdef MINEH_VULKAN
#include <glm/glm.hpp>
#include "../Context.hpp"
#include "../../Graphics/Drawable.hpp"

namespace mh
{
    namespace Vk
    {
        struct VkMesh : Mesh
        {
            Vk::Context* context = nullptr;
            std::pair<Vk::Texture*, std::string> texture = { nullptr, "" };
            std::string modelPath = "";
            uint32_t vertexID, indexID, pipelineID, descriptorID;
            
            VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
            VkPipeline graphicsPipeline = VK_NULL_HANDLE;
            
            uint32_t indicesCount;
            Vk::Buffer vertexBuffer, indexBuffer;
            Vk::Descriptor descriptor;
            
            glm::mat4 model;
            bool mDirty = false;
            
            VkMesh(Vk::Context* context = nullptr);
            
            void create() override;
            void destroy() override;
            void resize() override;
            void dirty() override;
            
            void record(const uint32_t& i) override;
            
            void setTexture(const std::string& path) override;
            void setModel(const std::string& path) override;
            void setContext(void* context) override;
            
            void updateModel(const bool& force = false);
            void createDescriptors();
            void createGraphicsPipeline();
        };
    }
}

#endif

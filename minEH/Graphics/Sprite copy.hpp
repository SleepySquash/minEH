//
//  Sprite.hpp
//  OpenGL App
//
//  Created by Никита Исаенко on 17.01.2021.
//

#pragma once

#include <glm/glm.hpp>

#include "../Renderer/Renderer.hpp"
#include "Drawable.hpp"

namespace mh
{
    namespace GL
    {
        struct Sprite
        {
            GL::Texture* texture = nullptr;
            GLuint VAO, VBO, EBO, ID, modelLoc;
            glm::mat4 model;
            bool dirty = true;
            
            float xcoef{ 1.f }, ycoef{ 1.f };
            glm::vec2 position{ 0.f, 0.f };
            glm::vec2 scale{ 1.f, 1.f };
            
            Sprite();
            Sprite(GL::Texture& t);
            Sprite(GL::Texture* t);
            ~Sprite();
            
            void setTexture(GL::Texture& t);
            void setTexture(GL::Texture* t);
            
            void create();
            void resize(const WindowSize& w);
            void draw(GL::Context& context);
            
            void setPosition(const glm::vec2& v);
            void setScale(const glm::vec2& v);
        };
    }
    
    namespace Vk
    {
        struct Sprite
        {
            float xcoef{ 1.f }, ycoef{ 1.f };
            
            VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
            VkPipeline graphicsPipeline = VK_NULL_HANDLE;
            VkPipelineCache pipelineCache = VK_NULL_HANDLE;
            
            VkShaderModule vertexShader = VK_NULL_HANDLE,
                           fragmentShader = VK_NULL_HANDLE;
            
            uint32_t vertexID, indexID, pipelineID;
            Vk::Buffer vertexBuffer, indexBuffer;
            Vk::Descriptor descriptor;
            Vk::Texture* texture = nullptr;
            
            Vk::UBO<glm::mat4> ubo;
            Vk::Model<glm::vec2> model;
            
            Vk::Context* context = nullptr;
            
            void create();
            void recreate();
            void resize(const WindowSize& w);
            void destroy();
            void onRecord(VkCommandBuffer& commandBuffer, uint32_t i);
            void updateUniformBuffers(const uint32_t& i, const bool& force = false);
            void dirty();
            void draw(Vk::Context& context);
            
            void recreateUniformBuffers();
            void createUniformBuffers();
            void createDescriptorSetLayout();
            void createDescriptors();
            void createGraphicsPipeline();
            
            void setTexture(Vk::Texture* t);
            void setContext(Context* context);
            
            void setPosition(const glm::vec2& pos);
            void setRotation(const float& rot);
            void setScale(const glm::vec2& scl);
            const glm::vec2& getPosition();
            const float& getRotation();
            const glm::vec2& getScale();
        };
    }
}

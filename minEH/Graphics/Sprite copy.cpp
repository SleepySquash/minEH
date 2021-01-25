//
//  Sprite.cpp
//  OpenGL App
//
//  Created by Никита Исаенко on 17.01.2021.
//

#include "Sprite.hpp"
#include "Collector/Shader.hpp"
#include "Collector/Buffer.hpp"
#include "Collector/Texture.hpp"
#include "Collector/Pipeline.hpp"
#include "../System/ResourcePath.hpp"

#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace mh
{
    namespace GL
    {
        Sprite::Sprite() { create(); }
        Sprite::Sprite(GL::Texture& t) : texture(&t) { create(); }
        Sprite::Sprite(GL::Texture* t) : texture( t) { create(); }
        Sprite::~Sprite()
        {
            if (ShaderCollection::sprite)
            {
                if (ShaderCollection::sprite->uses != 0) // TODO: shader collector
                    --ShaderCollection::sprite->uses;
            }
        }
        void Sprite::setTexture(GL::Texture& t) { texture = &t; }
        void Sprite::setTexture(GL::Texture* t) { texture =  t; }
        
        void Sprite::create()
        {
            GL::Buffer* buffer = (GL::Buffer*)bc::get("quad").first;
            VAO = buffer->VAO;
            VBO = buffer->VBO;
            
            if (ShaderCollection::sprite) ++ShaderCollection::sprite->uses;
            else ShaderCollection::sprite = new ShaderCollectorObject("Shaders/sprite.vert", "Shaders/sprite.frag");
            ID = ShaderCollection::sprite->shader.ID;
            modelLoc = glGetUniformLocation(ID, "model");
        }
        
        void Sprite::resize(const WindowSize& w)
        {
            float aspect = (float)w.width/w.height;
            float rwidth = texture->image.width, rheight = texture->image.height;
            if (aspect > 1.f) rwidth /= aspect; else rheight *= aspect;
            if (rwidth > rheight) { ycoef = ((float)rheight/rwidth); xcoef = 1.f; }
            else if (rwidth < rheight) { xcoef = ((float)rwidth/rheight); ycoef = 1.f; }
            dirty = true;
        }
        
        void Sprite::draw(GL::Context& context)
        {
    #ifdef MINEH_DEBUG
            if (!texture) { std::cout << "Error :: Sprite :: Draw :: No texture is bound (nullptr) while attempting to draw.\n"; return; }
    #endif
            if (context.shaderID != ID) { glUseProgram(ID); context.shaderID = ID; }
            if (context.VAOID != VAO) { glBindVertexArray(VAO); context.VAOID = VAO; }
            if (context.VBOID != VBO) { glBindBuffer(GL_ARRAY_BUFFER, VBO); context.VBOID = VBO; }
            if (dirty)
            {
                model = glm::mat4(1.f);
                model = glm::translate(model, glm::vec3(position.x, position.y, 0.f));
                model = glm::scale(model, glm::vec3(scale.x * xcoef, scale.y * ycoef, 0.f));
                dirty = false;
            }
            glUniformMatrix4fv(modelLoc, 1, false, glm::value_ptr(model));
            glActiveTexture(GL_TEXTURE0);
            if (context.textureID != texture->id) {
                glBindTexture(GL_TEXTURE_2D, texture->id); context.textureID = texture->id; }
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        
        void Sprite::setPosition(const glm::vec2& v) { position.x = v.x; position.y = -v.y; dirty = true; }
        void Sprite::setScale(const glm::vec2& v) { scale = v; dirty = true; }
    }
    
    
#pragma mark -
    
    namespace Vk
    {
        void Sprite::setContext(Context* context) { this->context = context; }
        void Sprite::destroy()
        {
            if (!context) return;
            for (auto& b : ubo.buffers) context->freeBuffer(b);
            bc::erase("quad-v");
            bc::erase("quad-i");
            pc::erase("sprite");
            context->freeDescriptor(descriptor);
        }
        void Sprite::create()
        {
            auto vb = bc::get("quad-v");
            vertexBuffer = *(Vk::Buffer*)vb.first; vertexID = vb.second;
            auto ib = bc::get("quad-i");
            indexBuffer = *(Vk::Buffer*)ib.first; indexID = ib.second;
            
            createUniformBuffers();
            createDescriptorSetLayout();
            createDescriptors();
            createGraphicsPipeline();
        }
        void Sprite::recreate()
        {
            vkDestroyDescriptorPool(context->device, descriptor.pool, nullptr);
            
            recreateUniformBuffers();
            createDescriptors();
            createGraphicsPipeline();
        }
        void Sprite::resize(const WindowSize& w)
        {
            float aspect = (float)w.width/w.height;
            float rwidth = texture->image.width, rheight = texture->image.height;
            if (aspect > 1.f) rwidth /= aspect; else rheight *= aspect;
            if (rwidth > rheight) { ycoef = ((float)rheight/rwidth); xcoef = 1.f; }
            else if (rwidth < rheight) { xcoef = ((float)rwidth/rheight); ycoef = 1.f; }
            dirty();
        }
        
        void Sprite::recreateUniformBuffers()
        {
            int difference = static_cast<int>(context->swapChainProps.images) - static_cast<int>(ubo.buffers.size());
            if (difference > 0)
            {
                ubo.buffers.resize(context->swapChainProps.images);
                for (size_t i = ubo.buffers.size() - difference; i < ubo.buffers.size(); ++i)
                    context->createBuffer(sizeof(CameraBufferObject), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, ubo.buffers[i].buffer, ubo.buffers[i].memory.memory);
            }
            else if (difference < 0)
            {
                for (size_t i = ubo.buffers.size() + difference; i > ubo.buffers.size() + difference; --i)
                    context->freeBuffer(ubo.buffers[i]);
                ubo.buffers.resize(context->swapChainProps.images);
            }
        }

        void Sprite::createUniformBuffers()
        {
            ubo.buffers.resize(context->swapChainProps.images);
            for (uint32_t i = 0; i < ubo.buffers.size(); ++i)
            {
                context->createBuffer(sizeof(ubo.model), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, ubo.buffers[i].buffer, ubo.buffers[i].memory.memory);
                updateUniformBuffers(i, true);
            }
        }

        void Sprite::createDescriptorSetLayout()
        {
            VkDescriptorSetLayoutBinding uboBinding;
            uboBinding.binding = 0;
            uboBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            uboBinding.descriptorCount = 1;
            uboBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
            uboBinding.pImmutableSamplers = nullptr;
            
            VkDescriptorSetLayoutBinding samplerBinding;
            samplerBinding.binding = 1;
            samplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            samplerBinding.descriptorCount = 1;
            samplerBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
            samplerBinding.pImmutableSamplers = nullptr;
            
            std::vector<VkDescriptorSetLayoutBinding> bindings = { uboBinding, samplerBinding };

            VkDescriptorSetLayoutCreateInfo createInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
            createInfo.bindingCount = static_cast<uint32_t>(bindings.size());
            createInfo.pBindings = bindings.data();
            if (vkCreateDescriptorSetLayout(context->device, &createInfo, nullptr, &descriptor.layout) != VK_SUCCESS) throw std::runtime_error("createDescriptorSetLayout() failed!");
        }
        void Sprite::createDescriptors()
        {
            // createDescriptorPool
            
            std::vector<VkDescriptorPoolSize> size(2);
            size[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            size[0].descriptorCount = context->swapChainProps.images;
            size[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            size[1].descriptorCount = context->swapChainProps.images;

            VkDescriptorPoolCreateInfo createInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
            createInfo.maxSets = context->swapChainProps.images;
            createInfo.poolSizeCount = static_cast<uint32_t>(size.size());
            createInfo.pPoolSizes = size.data();
            if (vkCreateDescriptorPool(context->device, &createInfo, nullptr, &descriptor.pool) != VK_SUCCESS) throw std::runtime_error("createDescriptorPool() failed!");
            
            // createDescriptorSets
            
            std::vector<VkDescriptorSetLayout> layouts(context->swapChainProps.images, descriptor.layout);

            VkDescriptorSetAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
            allocInfo.descriptorPool = descriptor.pool;
            allocInfo.descriptorSetCount = context->swapChainProps.images;
            allocInfo.pSetLayouts = layouts.data();
            descriptor.sets.resize(context->swapChainProps.images);
            if (vkAllocateDescriptorSets(context->device, &allocInfo, descriptor.sets.data()) != VK_SUCCESS) throw std::runtime_error("createDescriptorSets() failed!");

            for (size_t i = 0; i < descriptor.sets.size(); ++i)
            {
                std::vector<VkWriteDescriptorSet> writeSets(2);
                
                VkDescriptorBufferInfo bufferInfo;
                bufferInfo.buffer = ubo.buffers[i].buffer;
                bufferInfo.offset = ubo.buffers[i].offset;
                bufferInfo.range = sizeof(ubo.model);

                writeSets[0].sType = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
                writeSets[0].dstSet = descriptor.sets[i];
                writeSets[0].dstBinding = 0;
                writeSets[0].dstArrayElement = 0;
                writeSets[0].descriptorCount = 1;
                writeSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                writeSets[0].pImageInfo = nullptr;
                writeSets[0].pBufferInfo = &bufferInfo;
                writeSets[0].pTexelBufferView = nullptr;

                VkDescriptorImageInfo imageInfo;
                imageInfo.sampler = texture->sampler;
                imageInfo.imageView = texture->image.view;
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

                writeSets[1].sType = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
                writeSets[1].dstSet = descriptor.sets[i];
                writeSets[1].dstBinding = 1;
                writeSets[1].dstArrayElement = 0;
                writeSets[1].descriptorCount = 1;
                writeSets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                writeSets[1].pImageInfo = &imageInfo;
                writeSets[1].pBufferInfo = nullptr;
                writeSets[1].pTexelBufferView = nullptr;

                vkUpdateDescriptorSets(context->device, static_cast<uint32_t>(writeSets.size()), writeSets.data(), 0, nullptr);
            }
        }
        
        void Sprite::createGraphicsPipeline()
        {
            PipelineCollectorObject* pco = pc::get("sprite");
            if (!pco->loaded)
            {
                pco->vertexShaderPath = resourcePath() + "/Shaders/spv/_sprite.vert.spv";
                pco->fragmentShaderPath = resourcePath() + "/Shaders/spv/_sprite.frag.spv";
                pco->depthEnabled = false;
                pco->descriptor = &descriptor;
                
                pco->vBindingDescription.binding = 0;
                pco->vBindingDescription.stride = sizeof(Vertex<glm::vec2>);
                pco->vBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

                pco->vAttributeDescription.resize(2);
                
                pco->vAttributeDescription[0].binding = 0;
                pco->vAttributeDescription[0].location = 0;
                pco->vAttributeDescription[0].format = VK_FORMAT_R32G32_SFLOAT;
                pco->vAttributeDescription[0].offset = offsetof(Vertex<glm::vec2>, pos);

                pco->vAttributeDescription[1].binding = 0;
                pco->vAttributeDescription[1].location = 1;
                pco->vAttributeDescription[1].format = VK_FORMAT_R32G32_SFLOAT;
                pco->vAttributeDescription[1].offset = offsetof(Vertex<glm::vec2>, uv);
                
                pco->createPipeline(context);
            }
            graphicsPipeline = pco->pipeline;
            pipelineLayout = pco->layout;
            pipelineID = pco->id;
            return;
        }
        
        void Sprite::onRecord(VkCommandBuffer& commandBuffer, uint32_t i)
        {
            if (context->pipelineID != pipelineID)
            {
                vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
                context->pipelineID = pipelineID;
            }
            
            if (context->vertexID != vertexID)
            {
                VkDeviceSize offset = 0;
                vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer.buffer, &offset);
                context->vertexID = vertexID;
            }
            if (context->indexID != indexID)
            {
                vkCmdBindIndexBuffer(commandBuffer, indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
                context->indexID = indexID;
            }
            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptor.sets[i], 0, nullptr);
            
            vkCmdDrawIndexed(commandBuffer, 6, 1, 0, 0, 0);
        }

        void Sprite::updateUniformBuffers(const uint32_t& i, const bool& force)
        {
            // TODO: find a way to update the buffers outside the Draw() cycle (w/o it gets down from 30 ms to 17 ms!!!!!!!!)
            if (ubo.dirty || force)
            {
                ubo.model = glm::mat4(1.f);
                ubo.model = glm::translate(ubo.model, glm::vec3(model.position, 0.f));
                ubo.model = glm::rotate(ubo.model, model.rotation.x, glm::vec3(0.f, 0.f, 1.f));
                ubo.model = glm::scale(ubo.model, glm::vec3(model.scale.x * xcoef, model.scale.y * ycoef, 1.f));
                
                if (!ubo.buffers[i].memory.mapped)
                    vkMapMemory(context->device, ubo.buffers[i].memory.memory, 0, sizeof(ubo.model), 0, &ubo.buffers[i].memory.mapped);
                memcpy(ubo.buffers[i].memory.mapped, &ubo.model, sizeof(ubo.model));
                
                if (ubo.dirty) --ubo.dirty;
            }
        }

        void Sprite::dirty() { ubo.dirty = static_cast<uint32_t>(context->commandBuffers.size()); }

        void Sprite::setTexture(Vk::Texture* t) { texture = t; }
        
        void Sprite::setPosition(const glm::vec2& pos) { model.position = pos; dirty(); }
        void Sprite::setRotation(const float& rot) { model.rotation.x = rot; dirty(); }
        void Sprite::setScale(const glm::vec2& scl) { model.scale = scl; dirty(); }
        const glm::vec2& Sprite::getPosition() { return model.position; }
        const float& Sprite::getRotation() { return model.rotation.x; }
        const glm::vec2& Sprite::getScale() { return model.scale; }
    }
}

//
//  Sprite.cpp
//  OpenGL App
//
//  Created by Никита Исаенко on 17.01.2021.
//

#include "Sprite.hpp"

#ifdef MINEH_VULKAN
#include "../../Engine/Collector/Buffer.hpp"
#include "../../Engine/Collector/Texture.hpp"
#include "../../Engine/Collector/Pipeline.hpp"
#include "../../Engine/Collector/Descriptor.hpp"
#include "../../Support/ResourcePath.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace mh
{
    namespace Vk
    {
        VkSprite::VkSprite() { }
        VkSprite::VkSprite(Vk::Context* context) : context(context) { }
        
        void VkSprite::setContext(void* context) { this->context = (Vk::Context*)context; }
        void VkSprite::destroy()
        {
            if (!context) return;
            bc::erase("quad");
            pc::erase("sprite");
            dc::erase("texture " + texture.second);
            tc::erase(texture.second);
        }
        void VkSprite::create()
        {
            auto vb = bc::get("quad");
            vertexBuffer = *(Vk::Buffer*)vb->buffer; vertexID = vb->id;
            createDescriptors();
            createGraphicsPipeline();
        }
        void VkSprite::resize()
        {
            float aspect = (float)context->swapChainProps.extent.width/context->swapChainProps.extent.height;
            float rwidth = texture.first->image.width, rheight = texture.first->image.height;
            if (aspect > 1.f) rwidth /= aspect; else rheight *= aspect;
            if (rwidth > rheight) { ycoef = ((float)rheight/rwidth); xcoef = 1.f; }
            else if (rwidth < rheight) { xcoef = ((float)rwidth/rheight); ycoef = 1.f; }
            updateModel(true);
            
            createDescriptors();
            createGraphicsPipeline();
        }

        void VkSprite::createDescriptors()
        {
            DescriptorCollectorObject* dco = dc::get("texture " + texture.second);
            if (!dco->loaded)
            {
                VkDescriptorSetLayoutBinding samplerBinding;
                samplerBinding.binding = 0;
                samplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                samplerBinding.descriptorCount = 1;
                samplerBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
                samplerBinding.pImmutableSamplers = nullptr;

                dco->bindings.resize(1);
                dco->bindings[0] = samplerBinding;

                dco->size.resize(1);
                dco->size[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                dco->size[0].descriptorCount = context->swapChainProps.images;
                
                VkDescriptorImageInfo imageInfo;
                imageInfo.sampler = texture.first->sampler;
                imageInfo.imageView = texture.first->image.view;
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

                dco->writeSets.resize(1);
                dco->writeSets[0].sType = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
                dco->writeSets[0].dstBinding = 0;
                dco->writeSets[0].dstArrayElement = 0;
                dco->writeSets[0].descriptorCount = 1;
                dco->writeSets[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                dco->writeSets[0].pImageInfo = &imageInfo;
                dco->writeSets[0].pBufferInfo = nullptr;
                dco->writeSets[0].pTexelBufferView = nullptr;
                
                dco->createDescriptor(context);
            }
            descriptor = dco->descriptor;
            descriptorID = dco->id;
        }
        
        void VkSprite::createGraphicsPipeline()
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
                
                pco->pushConstantRanges.resize(1);
                pco->pushConstantRanges[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
                pco->pushConstantRanges[0].offset = 0;
                pco->pushConstantRanges[0].size = sizeof(glm::mat4);
                
                pco->createPipeline(context);
            }
            graphicsPipeline = pco->pipeline;
            pipelineLayout = pco->layout;
            pipelineID = pco->id;
            return;
        }
        
        void VkSprite::record(const uint32_t& i)
        {
            VkCommandBuffer& commandBuffer = context->commandBuffers[i];
            if (context->pipelineID != pipelineID) {
                vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline); context->pipelineID = pipelineID; }
            
            if (context->vertexID != vertexID) {
                VkDeviceSize offset = 0; vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer.buffer, &offset); context->vertexID = vertexID; }
            // if (context->indexID != indexID) { vkCmdBindIndexBuffer(commandBuffer, indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32); context->indexID = indexID; }
            
            if (context->descriptorID != descriptorID) {
                vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptor.sets[i], 0, nullptr);
                context->descriptorID = descriptorID; }
            
            vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &model);
            // vkCmdDrawIndexed(commandBuffer, 6, 1, 0, 0, 0);
            vkCmdDraw(commandBuffer, 6, 1, 0, 0);
        }

        void VkSprite::updateModel(const bool& force)
        {
            if (mDirty || force)
            {
                model = glm::mat4(1.f);
                model = glm::translate(model, glm::vec3(position, 0.f));
                model = glm::rotate(model, rotation, glm::vec3(0.f, 0.f, 1.f));
                model = glm::scale(model, glm::vec3(scale.x * xcoef, scale.y * ycoef, 1.f));
                mDirty = false;
            }
        }
        void VkSprite::dirty() { mDirty = true; updateModel(); }
        void VkSprite::setTexture(const std::string& path) { texture.first = (Vk::Texture*)tc::get(path); texture.second = path; }
    }
}

#endif

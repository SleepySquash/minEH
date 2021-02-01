//
//  Descriptor.hpp
//  OpenGL App
//
//  Created by Никита Исаенко on 22.01.2021.
//

#pragma once

#include "../../Support/Config.hpp"
#ifdef MINEH_VULKAN

#include <unordered_map>
#include <list>

#include "../../Renderer/Context.hpp"

namespace mh
{
    enum class DescriptorType { undef, Camera };
    
    // TODO: Tie to ImageView if descriptor contains one that is used in the TextureCollector so when ImageView gets destroyed there the DCO gets some kind flag of "needs to be recreated since ImageView is destroyed".
    struct DescriptorCollectorObject
    {
        Vk::Descriptor descriptor;
        
        uint32_t usage, id;
        bool loaded = false, destroyable = true;
        uint8_t frames;
        
        std::vector<VkDescriptorSetLayoutBinding> bindings;
        std::vector<VkDescriptorPoolSize> size;
        std::vector<VkWriteDescriptorSet> writeSets;
        std::vector<DescriptorType> types;
        
        void createDescriptor(Vk::Context* context);
        void createDescriptorLayout(Vk::Context* context);
        void createDescriptorPoolAndSets(Vk::Context* context);
        void recreateDescriptor(Vk::Context* context);
        void destroyDescriptor(Vk::Context* context);
        DescriptorCollectorObject(const uint32_t& usage = 0);
    };

    // Note that if UBO is set as one of the descriptors and the VkBuffer handle changes, you need to update it!
    // e.g. when resize() occurs and the size of UBOs changes -> you need to add the new one
    struct DescriptorCollector
    {
        static Vk::Context* context;
        static std::unordered_map<std::string, DescriptorCollectorObject*> map;
        static std::list<std::unordered_map<std::string, DescriptorCollectorObject*>::iterator> trash;
        static uint32_t ids;
        
        static void bindContext(Vk::Context* context);
        static DescriptorCollectorObject* get(const std::string& id);
        static DescriptorCollectorObject* raw(const std::string& id);
        static void erase(const std::string& id, const uint32_t& count = 1);
        static void frame();
        static void recreate();
        static void clear();
    };

    typedef DescriptorCollector dc;
}

#endif

/*
 
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

 

std::vector<VkDescriptorPoolSize> size(2);
size[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
size[0].descriptorCount = context->swapChainProps.images;
size[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
size[1].descriptorCount = context->swapChainProps.images;


 
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
 
*/

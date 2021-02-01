//
//  Descriptor.cpp
//  OpenGL App
//
//  Created by Никита Исаенко on 22.01.2021.
//

#include "Descriptor.hpp"

#ifdef MINEH_VULKAN
#include <iostream>
#include "../../Renderer/Vulkan/Camera.hpp"

namespace mh
{
    std::unordered_map<std::string, DescriptorCollectorObject*> DescriptorCollector::map;
    std::list<std::unordered_map<std::string, DescriptorCollectorObject*>::iterator> DescriptorCollector::trash;
    Vk::Context* DescriptorCollector::context = nullptr;
    uint32_t DescriptorCollector::ids = 1;
    
    
#pragma mark -
#pragma mark Descriptor Collector Object
    
    DescriptorCollectorObject::DescriptorCollectorObject(const uint32_t& usage) : usage(usage) { }
    void DescriptorCollectorObject::createDescriptor(Vk::Context* context)
    {
        createDescriptorLayout(context);
        createDescriptorPoolAndSets(context);
        loaded = true;
    }
    void DescriptorCollectorObject::createDescriptorLayout(Vk::Context *context)
    {
        VkDescriptorSetLayoutCreateInfo createInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
        createInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        createInfo.pBindings = bindings.data();
        if (vkCreateDescriptorSetLayout(context->device, &createInfo, nullptr, &descriptor.layout) != VK_SUCCESS) throw std::runtime_error("createDescriptorSetLayout() failed!");
    }
    void DescriptorCollectorObject::createDescriptorPoolAndSets(Vk::Context *context)
    {
        VkDescriptorPoolCreateInfo createInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
        createInfo.maxSets = context->swapChainProps.images;
        createInfo.poolSizeCount = static_cast<uint32_t>(size.size());
        createInfo.pPoolSizes = size.data();
        if (vkCreateDescriptorPool(context->device, &createInfo, nullptr, &descriptor.pool) != VK_SUCCESS) throw std::runtime_error("createDescriptorPool() failed!");

        std::vector<VkDescriptorSetLayout> layouts(context->swapChainProps.images, descriptor.layout);
        
        VkDescriptorSetAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
        allocInfo.descriptorPool = descriptor.pool;
        allocInfo.descriptorSetCount = context->swapChainProps.images;
        allocInfo.pSetLayouts = layouts.data();
        descriptor.sets.resize(context->swapChainProps.images);
        if (vkAllocateDescriptorSets(context->device, &allocInfo, descriptor.sets.data()) != VK_SUCCESS) throw std::runtime_error("createDescriptorSets() failed!");

        std::vector<VkDescriptorBufferInfo*> bufferInfoToDestroy;
        for (size_t i = 0; i < descriptor.sets.size(); ++i)
        {
            for (size_t j = 0; j < writeSets.size(); ++j)
            {
                writeSets[j].dstSet = descriptor.sets[i];
                if (types.size() > j)
                {
                    if (types[j] == DescriptorType::Camera)
                    {
                        VkDescriptorBufferInfo* bufferInfo = new VkDescriptorBufferInfo;
                        bufferInfo->buffer = ((Vk::VkCamera*)context->camera)->uniformBuffers[i].buffer;
                        bufferInfo->offset = 0;
                        bufferInfo->range = sizeof(CameraBufferObject);
                        
                        writeSets[j].pBufferInfo = bufferInfo;
                        bufferInfoToDestroy.push_back(bufferInfo);
                    }
                }
            }
            vkUpdateDescriptorSets(context->device, static_cast<uint32_t>(writeSets.size()), writeSets.data(), 0, nullptr);
            if (bufferInfoToDestroy.size()) { for (auto c : bufferInfoToDestroy) delete c; bufferInfoToDestroy.clear(); }
        }
    }
    void DescriptorCollectorObject::recreateDescriptor(Vk::Context *context)
    {
        if (descriptor.sets.size() != context->swapChainProps.images)
        {
            vkDestroyDescriptorPool(context->device, descriptor.pool, nullptr);
            createDescriptorPoolAndSets(context);
        }
    }
    void DescriptorCollectorObject::destroyDescriptor(Vk::Context *context)
    {
        vkDestroyDescriptorPool(context->device, descriptor.pool, nullptr);
        vkDestroyDescriptorSetLayout(context->device, descriptor.layout, nullptr);
    }
    
    
#pragma mark -
#pragma mark Descriptor Collector
    
    void DescriptorCollector::bindContext(Vk::Context* context) { DescriptorCollector::context = context; }
    DescriptorCollectorObject* DescriptorCollector::get(const std::string& id)
    {
        // TODO: Fix ++usage every time get() invoked.
        auto it = map.find(id);
        if (it != map.end())
        {
            ++(*it).second->usage;
            if ((*it).second->frames)
            {
                (*it).second->frames = 0;
                for (auto iter = trash.begin(); iter != trash.end(); ++iter)
                    if (*iter == it) { trash.erase(iter); break; }
            }
            return (*it).second;
        }
        else
        {
            it = map.emplace(id, nullptr).first;
            (*it).second = new DescriptorCollectorObject(1);
            (*it).second->id = ids++;
            return (*it).second;
        }
    }
    DescriptorCollectorObject* DescriptorCollector::raw(const std::string& id)
    {
        if (map.find(id) == map.end()) return nullptr;
        else
        {
            DescriptorCollectorObject* dco = get(id);
            --dco->usage; return dco;
        }
    }

    void DescriptorCollector::erase(const std::string& id, const uint32_t& count)
    {
        auto it = map.find(id);
        if (it != map.end())
        {
            if ((*it).second->usage == 1 || (*it).second->usage == 0)
            {
                if ((*it).second->destroyable)
                {
                    (*it).second->frames = context->commandBuffers.size() + 1;
                    trash.push_back(it);
                }
                else (*it).second->usage = 0;
            }
            else --(*it).second->usage;
        }
    }
    
    void DescriptorCollector::frame()
    {
        for (auto it = trash.begin(); it != trash.end();)
        {
            --((**it).second->frames);
            if ((**it).second->frames == 0)
            {
                if ((**it).second->loaded) (**it).second->destroyDescriptor(context);
                delete (**it).second; map.erase(*it); it = trash.erase(it);
            }
            else ++it;
        }
    }
    
    void DescriptorCollector::recreate()
    {
        for (auto it = map.begin(); it != map.end(); ++it)
            if ((*it).second->loaded) (*it).second->recreateDescriptor(context);
    }
    
    void DescriptorCollector::clear()
    {
        for (auto it = map.begin(); it != map.end(); ++it)
            if ((*it).second->loaded) { (*it).second->destroyDescriptor(context); delete (*it).second; }
        map.clear(); ids = 1;
    }
}

#endif

//
//  Camera.cpp
//  minEH
//
//  Created by Никита Исаенко on 24.01.2021.
//

#include "Camera.hpp"

#ifdef MINEH_VULKAN

namespace mh
{
    namespace Vk
    {
        VkCamera::VkCamera(Vk::Context* context) : context(context) { }
        void VkCamera::create()
        {
            uniformBuffers.resize(context->swapChainProps.images);
            for (size_t i = 0; i < uniformBuffers.size(); ++i)
                context->createBuffer(sizeof(CameraBufferObject), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, uniformBuffers[i].buffer, uniformBuffers[i].memory.memory);
            
            cbo.view = glm::lookAt(position, position - direction, up);
            dirty();
        }
        
        void VkCamera::destroy() { if (context) for (auto& b : uniformBuffers) context->freeBuffer(b); }
        void VkCamera::resize()
        {
            int difference = static_cast<int>(context->swapChainProps.images) - static_cast<int>(uniformBuffers.size());
            if (difference > 0)
            {
                uniformBuffers.resize(context->swapChainProps.images);
                for (size_t i = uniformBuffers.size() - difference; i < uniformBuffers.size(); ++i)
                    context->createBuffer(sizeof(CameraBufferObject), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, uniformBuffers[i].buffer, uniformBuffers[i].memory.memory);
            }
            else if (difference < 0)
            {
                for (size_t i = uniformBuffers.size() + difference; i > uniformBuffers.size() + difference; --i)
                    context->freeBuffer(uniformBuffers[i]);
                uniformBuffers.resize(context->swapChainProps.images);
            }
            
            cbo.proj = glm::perspective(fov, context->swapChainProps.extent.width / (float)context->swapChainProps.extent.height, near, far);
            cbo.proj[1][1] *= -1;
            
            dirty(); for (uint32_t i = 0; i < context->commandBuffers.size(); ++i) draw(i);
        }
        
        void VkCamera::update() { cbo.view = glm::lookAt(position, position - direction, up); dirty(); }
        void VkCamera::draw(const uint32_t& i)
        {
            if (dirtyBuffers)
            {
                if (!uniformBuffers[i].memory.mapped)
                    vkMapMemory(context->device, uniformBuffers[i].memory.memory, 0, sizeof(CameraBufferObject), 0, &uniformBuffers[i].memory.mapped);
                memcpy(uniformBuffers[i].memory.mapped, &cbo, sizeof(CameraBufferObject));
                
                if (dirtyBuffers) --dirtyBuffers;
            }
        }
        
        // TODO: Fix some buffers not being rewritten on the fly
        void VkCamera::dirty() { dirtyBuffers = static_cast<uint32_t>(context->commandBuffers.size() * 2); }
        void VkCamera::setContext(void* context) { this->context = (Vk::Context*)context; }
    }
}

#endif

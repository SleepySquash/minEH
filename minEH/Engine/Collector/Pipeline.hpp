//
//  Pipeline.hpp
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
    struct PipelineCollectorObject
    {
        VkPipeline pipeline;
        VkPipelineLayout layout;
        
        uint32_t usage, id;
        bool loaded = false, shaders = false, destroyable = true;
        uint8_t frames;
        
        //...
        std::string vertexShaderPath;
        std::string fragmentShaderPath;
        VkShaderModule vertexShader;
        VkShaderModule fragmentShader;
        
        std::vector<VkVertexInputAttributeDescription> vAttributeDescription;
        VkVertexInputBindingDescription vBindingDescription;
        uint32_t vBindingDescriptionCount = 1;
        
        Vk::Descriptor* descriptor = nullptr;
        bool depthEnabled = true;
        VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL;
        VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT;
        VkFrontFace frontFace = VK_FRONT_FACE_CLOCKWISE;
        
        std::vector<VkPushConstantRange> pushConstantRanges;
        
        void createPipeline(Vk::Context* context);
        void recreatePipeline(Vk::Context* context);
        void destroyPipeline(Vk::Context* context);
        PipelineCollectorObject(const uint32_t& usage = 0);
    };

    struct PipelineCollector
    {
        static Vk::Context* context;
        static std::unordered_map<std::string, PipelineCollectorObject*> map;
        static std::list<std::unordered_map<std::string, PipelineCollectorObject*>::iterator> trash;
        static uint32_t ids;
        
        static void bindContext(Vk::Context* context);
        static PipelineCollectorObject* get(const std::string& id);
        static PipelineCollectorObject* raw(const std::string& id);
        static void erase(const std::string& id, const uint32_t& count = 1);
        static void frame();
        static void recreate();
        static void clear();
    };

    typedef PipelineCollector pc;
}

#endif

/*
 
VkVertexInputBindingDescription vBindingDescription = {};
vBindingDescription.binding = 0;
vBindingDescription.stride = sizeof(Vertex<glm::vec2>);
vBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

std::vector<VkVertexInputAttributeDescription> vAttributeDescription(2);
vAttributeDescription[0].binding = 0;
vAttributeDescription[0].location = 0;
vAttributeDescription[0].format = VK_FORMAT_R32G32_SFLOAT;
vAttributeDescription[0].offset = offsetof(Vertex<glm::vec2>, pos);

vAttributeDescription[1].binding = 0;
vAttributeDescription[1].location = 1;
vAttributeDescription[1].format = VK_FORMAT_R32G32_SFLOAT;
vAttributeDescription[1].offset = offsetof(Vertex<glm::vec2>, uv);
 
*/

//
//  Pipeline.cpp
//  OpenGL App
//
//  Created by Никита Исаенко on 22.01.2021.
//

#include "Pipeline.hpp"

#ifdef MINEH_VULKAN
namespace mh
{
    std::unordered_map<std::string, PipelineCollectorObject> pc::map;
    Vk::Context* pc::context = nullptr;
    uint32_t pc::ids = 1;
    
    
#pragma mark -
#pragma mark Pipeline Collector Object
    
    PipelineCollectorObject::PipelineCollectorObject(const uint32_t& usage) : usage(usage) { }
    void PipelineCollectorObject::createPipeline(Vk::Context* context)
    {
        if (!shaders)
        {
            vertexShader = context->loadShader(vertexShaderPath);
            fragmentShader = context->loadShader(fragmentShaderPath);
            shaders = true;
        }
        
        VkPipelineShaderStageCreateInfo vertexStageInfo = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
        vertexStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertexStageInfo.module = vertexShader;
        vertexStageInfo.pName = "main";
        
        VkPipelineShaderStageCreateInfo fragmentStageInfo = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
        fragmentStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragmentStageInfo.module = fragmentShader;
        fragmentStageInfo.pName = "main";
        
        std::vector<VkPipelineShaderStageCreateInfo> vStages = { vertexStageInfo, fragmentStageInfo };

        
        VkPipelineVertexInputStateCreateInfo inputStateInfo = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
        inputStateInfo.vertexBindingDescriptionCount = vBindingDescriptionCount;
        inputStateInfo.pVertexBindingDescriptions = &vBindingDescription;
        inputStateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vAttributeDescription.size());
        inputStateInfo.pVertexAttributeDescriptions = vAttributeDescription.data();
        
        
        VkPipelineInputAssemblyStateCreateInfo assemblyStateInfo = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
        assemblyStateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        assemblyStateInfo.primitiveRestartEnable = false;
        
        
        VkViewport viewport;
        viewport.x = 0;
        viewport.y = 0;
        viewport.width = context->swapChainProps.extent.width;
        viewport.height = context->swapChainProps.extent.height;
        viewport.minDepth = 0;
        viewport.maxDepth = 1.f;
        
        VkRect2D scissors;
        scissors.offset = { 0, 0 };
        scissors.extent = context->swapChainProps.extent;
        
        VkPipelineViewportStateCreateInfo viewportStateInfo = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
        viewportStateInfo.viewportCount = 1;
        viewportStateInfo.pViewports = &viewport;
        viewportStateInfo.scissorCount = 1;
        viewportStateInfo.pScissors = &scissors;
        
        
        VkPipelineRasterizationStateCreateInfo rasterizationStateInfo = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
        rasterizationStateInfo.depthClampEnable = false;
        rasterizationStateInfo.rasterizerDiscardEnable = false;
        rasterizationStateInfo.polygonMode = polygonMode;
        rasterizationStateInfo.cullMode = cullMode;
        rasterizationStateInfo.frontFace = frontFace;
        rasterizationStateInfo.depthBiasEnable = false;
        rasterizationStateInfo.depthBiasConstantFactor = 0;
        rasterizationStateInfo.depthBiasClamp = 0;
        rasterizationStateInfo.depthBiasSlopeFactor = 0;
        rasterizationStateInfo.lineWidth = 1.f;
        
        
        VkPipelineMultisampleStateCreateInfo multisampleStateInfo = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
        multisampleStateInfo.rasterizationSamples = context->msaaSamples;
        multisampleStateInfo.sampleShadingEnable = true;
        multisampleStateInfo.minSampleShading = 1.f;
        multisampleStateInfo.pSampleMask = nullptr;
        multisampleStateInfo.alphaToCoverageEnable = false;
        multisampleStateInfo.alphaToOneEnable = false;
        
        
        VkPipelineDepthStencilStateCreateInfo depthStateInfo = { VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
        depthStateInfo.depthTestEnable = depthEnabled;
        depthStateInfo.depthWriteEnable = depthEnabled;
        depthStateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
        depthStateInfo.depthBoundsTestEnable = false;
        depthStateInfo.stencilTestEnable = false;
        depthStateInfo.front.failOp = VK_STENCIL_OP_KEEP;
        depthStateInfo.front.passOp = VK_STENCIL_OP_KEEP;
        depthStateInfo.front.compareOp = VK_COMPARE_OP_ALWAYS;
        depthStateInfo.back = depthStateInfo.front;
        depthStateInfo.minDepthBounds = 0;
        depthStateInfo.maxDepthBounds = 1;
        
        
        VkPipelineColorBlendAttachmentState attachmentState = {};
        attachmentState.blendEnable = true;
        attachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        attachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        attachmentState.colorBlendOp = VK_BLEND_OP_ADD;
        attachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        attachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        attachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
        attachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        
        VkPipelineColorBlendStateCreateInfo colorBlendStateInfo = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
        colorBlendStateInfo.logicOpEnable = false;
        colorBlendStateInfo.logicOp = VK_LOGIC_OP_COPY;
        colorBlendStateInfo.attachmentCount = 1;
        colorBlendStateInfo.pAttachments = &attachmentState;
        colorBlendStateInfo.blendConstants[0] = 0.f;
        colorBlendStateInfo.blendConstants[1] = 0.f;
        colorBlendStateInfo.blendConstants[2] = 0.f;
        colorBlendStateInfo.blendConstants[3] = 0.f;
        
        
        VkPipelineDynamicStateCreateInfo dynamicStateInfo = { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
        dynamicStateInfo.dynamicStateCount = 0;
        dynamicStateInfo.pDynamicStates = nullptr;
        
        
        VkPipelineLayoutCreateInfo layoutInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
        layoutInfo.setLayoutCount = descriptor ? 1 : 0;
        layoutInfo.pSetLayouts = descriptor ? &descriptor->layout : nullptr;
        layoutInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
        layoutInfo.pPushConstantRanges = pushConstantRanges.data();
        
        if (vkCreatePipelineLayout(context->device, &layoutInfo, nullptr, &layout) != VK_SUCCESS)
            throw std::runtime_error("vkCreatePipelineLayout() failed!");
        
        
        VkGraphicsPipelineCreateInfo createInfo = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
        createInfo.stageCount = static_cast<uint32_t>(vStages.size());
        createInfo.pStages = vStages.data();
        createInfo.pVertexInputState = &inputStateInfo;
        createInfo.pInputAssemblyState = &assemblyStateInfo;
        createInfo.pTessellationState = nullptr;
        createInfo.pViewportState = &viewportStateInfo;
        createInfo.pRasterizationState = &rasterizationStateInfo;
        createInfo.pMultisampleState = &multisampleStateInfo;
        createInfo.pDepthStencilState = &depthStateInfo;
        createInfo.pColorBlendState = &colorBlendStateInfo;
        createInfo.pDynamicState = &dynamicStateInfo;
        createInfo.layout = layout;
        createInfo.renderPass = context->renderPass;
        createInfo.subpass = 0;
        createInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
        createInfo.basePipelineIndex = -1; // Optional
        
        if (vkCreateGraphicsPipelines(context->device, VK_NULL_HANDLE, 1, &createInfo, nullptr, &pipeline) != VK_SUCCESS)
            throw std::runtime_error("createGraphicsPipeline() failed!");
        loaded = true;
    }
    void PipelineCollectorObject::recreatePipeline(Vk::Context *context)
    {
        vkDestroyPipeline(context->device, pipeline, nullptr);
        vkDestroyPipelineLayout(context->device, layout, nullptr);
        createPipeline(context);
    }
    void PipelineCollectorObject::destroyPipeline(Vk::Context *context)
    {
        vkDestroyPipeline(context->device, pipeline, nullptr);
        vkDestroyPipelineLayout(context->device, layout, nullptr);
        if (shaders)
        {
            vkDestroyShaderModule(context->device, vertexShader, nullptr);
            vkDestroyShaderModule(context->device, fragmentShader, nullptr);
        }
    }
    
    
#pragma mark -
#pragma mark Pipeline Collector
    
    void pc::bindContext(Vk::Context* context) { pc::context = context; }
    PipelineCollectorObject* pc::get(const std::string& id)
    {
        auto it = map.find(id);
        if (it != map.end())
        {
            ++(*it).second.usage;
            return &(*it).second;
        }
        else
        {
            it = map.emplace(id, 1).first;
            (*it).second.id = ids++;
            return &(*it).second;
        }
    }

    void pc::erase(const std::string& id, const uint32_t& count)
    {
        auto it = map.find(id);
        if (it != map.end())
        {
            if ((*it).second.usage == 1 || (*it).second.usage == 0)
            {
                if ((*it).second.destroyable)
                {
                    (*it).second.destroyPipeline(context);
                    map.erase(it);
                }
                else (*it).second.usage = 0;
            }
            else --(*it).second.usage;
        }
    }
    
    void pc::recreate()
    {
        for (auto it = map.begin(); it != map.end(); ++it)
            if ((*it).second.loaded) (*it).second.recreatePipeline(context);
    }
    
    void pc::clear()
    {
        for (auto it = map.begin(); it != map.end(); ++it)
            if ((*it).second.loaded) (*it).second.destroyPipeline(context);
        map.clear(); ids = 1;
    }
}

#endif

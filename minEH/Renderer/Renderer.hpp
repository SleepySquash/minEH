//
//  Renderer.hpp
//  minEH
//
//  Created by Никита Исаенко on 19.05.2021.
//

#pragma once

#include <stb_image.h>

#include "../Support/Config.hpp"
#include "../Graphics/Camera.hpp"
#include "../Window/Window.hpp"
#include "Vertex.hpp"

namespace mh
{
    struct Renderer
    {
        enum class Type { undef, GL, Vk, MTL, WebGL, DX, DX12 } type;
        Window* window = nullptr;
        Camera* camera = nullptr;
    };
    
    struct CameraBufferObject { glm::mat4 view, proj; };
    
    struct Image
    {
        uint32_t width{ 0 }, height{ 0 };
        stbi_uc* data{ nullptr };
        
        ~Image();
        Image();
        Image(const std::string& path, int reqComp = STBI_rgb_alpha);
        bool loadFromFile(const std::string& path, int reqComp = STBI_rgb_alpha);
        void free();
    };
    enum class TextureFilter { LINEAR, NEAREST, CUBIC };
    struct Texture
    {
        virtual void allocate(void* data, uint32_t width, uint32_t height, TextureFilter filter = TextureFilter::LINEAR);
        void loadFromImage(Image* image, TextureFilter filter = TextureFilter::LINEAR);
        virtual void free();
    };
    
    enum class BufferType { Array, Vertex, Index, UV };
    enum class BufferUpdateType { Static, Dynamic };
    struct Buffer
    {
        BufferType type;
        BufferUpdateType updateType;
        
        virtual void allocate(void* data, uint32_t size);
        virtual void update(void* data, uint32_t size);
        virtual void free();
    };
    
    struct RenderPass
    {
        
    };
    
    struct Framebuffer
    {
        
    };
    
    enum class ShaderStage { Vertex, Fragment, Geometry, Tessellation, Compute, Mesh, Task };
    
    enum class DescriptorType { Sampler, CombinedImageSampler, SampledImage, StorageImage, UniformTexelBuffer, StorageTexelBuffer, UniformBuffer, StorageBuffer, UniformBufferDynamic, StorageBufferDynamic, InputAttachment };
    struct DescriptorLayout {
        DescriptorType type;
        uint32_t binding;
        ShaderStage stage;
        Texture* texture;
        Buffer* buffer;
        DescriptorLayout(const DescriptorType& type, const uint32_t& binding, const ShaderStage& stage, Texture* texture = nullptr, Buffer* buffer = nullptr);
    };
    struct Descriptor
    {
        std::vector<DescriptorLayout> layouts;
        virtual void allocate();
        virtual void free();
        virtual void onRecord(const uint32_t& i);
    };
    
    struct Shader
    {
        std::string main;
        ShaderStage stage;
        virtual bool loadFromFile(const std::string& path);
        virtual void free();
    };
    
    
    enum class PipelineFrontFace { CounterClockwise, Clockwise };
    enum class PipelineTopology { PointList, LineList, LineStrip, TriangleList, TriangleStrip, TriangleFan, LineListWithAdjacency, LineStripWithAdjacency, TriangleListWithAdjacency, TriangleStripWithAdjacency, PatchList };
    enum class PipelinePolygonMode { Fill, Line, Point, FillRectangleNV };
    enum class PipelineCullMode { None, Front, Back, Both };
    
    enum class VertexFormat { UNDEFINED, R32G32_SFLOAT, R32G32B32_SFLOAT, R32G32B32A32_SFLOAT };
    struct PipelineAttribute {
        uint32_t location, binding;
        uint64_t offset;
        VertexFormat format;
        Buffer* buffer;
        PipelineAttribute(const uint32_t& location = 0, const uint32_t& binding = 0, const uint32_t& offset = 0, const VertexFormat& format = VertexFormat::UNDEFINED, Buffer* buffer = nullptr);
    };
    struct PipelineBinding {
        uint32_t binding, stride;
        PipelineBinding(const uint32_t& binding = 0, const uint32_t& stride = 0);
    };
    struct PipelinePushConstantRange {
        uint32_t stageFlags, offset, size;
        PipelinePushConstantRange(const uint32_t& stageFlags = 0, const uint32_t& offset = 0, const uint32_t& size = 0);
    };
    
    struct Pipeline
    {
        uint32_t pipelineID;
        
        std::vector<Shader*> shaders;
        std::vector<PipelineAttribute> attributes;
        std::vector<PipelineBinding> bindings;
        std::vector<PipelinePushConstantRange> pushConstantRanges;
        
        bool depthEnabled = true;
        Descriptor* descriptor = nullptr;
        PipelinePolygonMode polygonMode = PipelinePolygonMode::Fill;
        PipelineCullMode cullMode = PipelineCullMode::Back;
        PipelineFrontFace frontFace = PipelineFrontFace::CounterClockwise;
        PipelineTopology topology = PipelineTopology::TriangleList;
        
        virtual void allocate();
        virtual void free();
        virtual void onRecord(const uint32_t& i);
    };
}


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
        bool UV_0{ 0 }, UV_1{ 1 };
        glm::mat4 ortho;
        
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
        uint32_t width, height;
        virtual void allocate(void* data, uint32_t width, uint32_t height, TextureFilter filter = TextureFilter::LINEAR);
        void loadFromImage(Image* image, TextureFilter filter = TextureFilter::LINEAR);
        virtual void free();
    };
    
    enum class BufferType { Array, Vertex, Index };
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
        DescriptorLayout(const DescriptorType& type, const uint32_t& binding, const ShaderStage& stage);
    };
    struct Pipeline;
    struct Descriptor
    {
        std::vector<DescriptorLayout> layouts;
        bool created{ false };
        
        virtual void allocate();
        void allocate(const std::vector<void*>& data);
        virtual void update(const std::vector<void*>& data);
        virtual void free();
        virtual void onRecord(Pipeline* pipeline);
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
        PipelineAttribute(const uint32_t& location = 0, const uint32_t& binding = 0, const uint32_t& offset = 0, const VertexFormat& format = VertexFormat::UNDEFINED);
    };
    struct PipelineBinding {
        uint32_t binding, stride;
        PipelineBinding(const uint32_t& binding = 0, const uint32_t& stride = 0);
    };
    struct PipelinePushConstantRange {
        std::string name;
        ShaderStage stage;
        uint32_t offset, size;
        PipelinePushConstantRange(const ShaderStage& stage = ShaderStage::Vertex, const std::string& name = "", const uint32_t& offset = 0, const uint32_t& size = 0);
    };
    
    struct Pipeline
    {
        uint32_t pipelineID;
        uint32_t page = 0;
        
        std::vector<Shader*> shaders;
        std::vector<PipelineAttribute> attributes;
        std::vector<PipelineBinding> bindings;
        std::vector<PipelinePushConstantRange> pushConstantRanges;
        
        bool depthEnabled = false;
        Descriptor* descriptor = nullptr;
        PipelinePolygonMode polygonMode = PipelinePolygonMode::Fill;
        PipelineCullMode cullMode = PipelineCullMode::Back;
        PipelineFrontFace frontFace = PipelineFrontFace::Clockwise;
        PipelineTopology topology = PipelineTopology::TriangleList;
        
        virtual void allocate();
        virtual void recreate();
        virtual void free();
        virtual void onRecord(const uint32_t& i);
        virtual void vertex(const std::vector<Buffer*>& buffers);
        virtual void index(Buffer* buffer);
        virtual void push(const uint32_t& i, void* data);
        virtual void drawIndexed(const uint32_t& vertices, const uint32_t& indices);
    };
}


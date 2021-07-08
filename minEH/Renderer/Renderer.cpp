//
//  Renderer.cpp
//  minEH
//
//  Created by Никита Исаенко on 19.05.2021.
//

#include "Renderer.hpp"
#include <stb_image.h>

namespace mh
{
    void Buffer::allocate(void* data, uint32_t size) { }
    void Buffer::update(void* data, uint32_t size) { }
    void Buffer::free() { }
    
    Image::~Image() { free(); }
    Image::Image() { }
    Image::Image(const std::string& path, int reqComp) { loadFromFile(path, reqComp); }
    bool Image::loadFromFile(const std::string& path, int reqComp)
    {
        int _width, _height, comp;
        data = stbi_load(path.c_str(), &_width, &_height, &comp, reqComp);
        width = static_cast<uint32_t>(_width); height = static_cast<uint32_t>(_height);
        return data != nullptr;
    }
    void Image::free() { if (data) stbi_image_free(data); data = nullptr; }
    
    void Texture::allocate(void* data, uint32_t width, uint32_t height, TextureFilter filter) { }
    void Texture::loadFromImage(Image* image, TextureFilter filter) { allocate(image->data, image->width, image->height, filter); }
    void Texture::free() { }
    
    DescriptorLayout::DescriptorLayout(const DescriptorType& type, const uint32_t& binding, const ShaderStage& stage) : type(type), binding(binding), stage(stage) { };
    
    void Descriptor::allocate() { }
    void Descriptor::allocate(const std::vector<void*>& data) { allocate(); update(data); }
    void Descriptor::update(const std::vector<void*>& data) { }
    void Descriptor::free() { }
    void Descriptor::onRecord(Pipeline* pipeline) { }
    
    bool Shader::loadFromFile(const std::string& path) { return false; }
    void Shader::free() { }
    
    PipelineAttribute::PipelineAttribute(const uint32_t& location, const uint32_t& binding, const uint32_t& offset, const VertexFormat& format) : location(location), binding(binding), offset(offset), format(format) { }
    PipelineBinding::PipelineBinding(const uint32_t& binding, const uint32_t& stride) : binding(binding), stride(stride) { }
    PipelinePushConstantRange::PipelinePushConstantRange(const ShaderStage& stage, const std::string& name, const uint32_t& offset, const uint32_t& size) : stage(stage), name(name), offset(offset), size(size) { }
    
    void Pipeline::allocate() { }
    void Pipeline::recreate() { }
    void Pipeline::free() { }
    void Pipeline::onRecord(const uint32_t& i) { }
    void Pipeline::vertex(const std::vector<Buffer*>& buffers) { }
    void Pipeline::index(Buffer* buffer) { }
    void Pipeline::push(const uint32_t& i, void* data) { }
    void Pipeline::drawIndexed(const uint32_t& vertices, const uint32_t& indices) { }
}

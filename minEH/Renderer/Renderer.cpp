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
    
    bool Shader::loadFromFile(const std::string& path) { return false; }
    void Shader::free() { }
    
    PipelineAttribute::PipelineAttribute(const uint32_t& location, const uint32_t& binding, const uint32_t& offset, const VertexFormat& format) : location(location), binding(binding), offset(offset), format(format) { }
    PipelineBinding::PipelineBinding(const uint32_t& binding, const uint32_t& stride) : binding(binding), stride(stride) { }
    PipelinePushConstantRange::PipelinePushConstantRange(const uint32_t& stageFlags, const uint32_t& offset, const uint32_t& size) : stageFlags(stageFlags), offset(offset), size(size) { }
    
    void Pipeline::allocate() { }
    void Pipeline::free() { }
    void Pipeline::onRecord(const uint32_t& i) { }
}

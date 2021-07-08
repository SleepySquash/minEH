//
//  Factory.cpp
//  minEH
//
//  Created by Никита Исаенко on 19.05.2021.
//

#include "Factory.hpp"

namespace mh
{
    namespace Renders
    {
        mh::Buffer* Buffer(Renderer* context, const BufferType& type, void* data, const uint32_t& size, const BufferUpdateType& updateType)
        {
            mh::Buffer* buffer = nullptr;
            switch (context->type)
            {
                default: return nullptr; break;
#ifdef MINEH_OPENGL
                case Renderer::Type::GL: buffer = new GL::Buffer((GL::Context*)context); break;
#endif
#ifdef MINEH_VULKAN
                case Renderer::Type::Vk: buffer = new Vk::Buffer((Vk::Context*)context); break;
#endif
            }
            buffer->type = type;
            buffer->updateType = updateType;
            buffer->allocate(data, size);
            return buffer;
        }
        
        mh::Texture* Texture(Renderer* context, const std::string& path)
        {
            mh::Texture* texture = nullptr;
            switch (context->type)
            {
                default: return nullptr; break;
#ifdef MINEH_OPENGL
                case Renderer::Type::GL: texture = new GL::Texture((GL::Context*)context); break;
#endif
#ifdef MINEH_VULKAN
                case Renderer::Type::Vk: texture = new Vk::Texture((Vk::Context*)context); break;
#endif
            }
            if (path != "") {
                Image image(path);
                texture->allocate(image.data, image.width, image.height);
            }
            return texture;
        }
        
        mh::Shader* Shader(Renderer* context, const ShaderStage& stage, const std::string& path, const std::string& subpath, const std::string& main)
        {
            mh::Shader* shader = nullptr;
            switch (context->type)
            {
                default: return nullptr; break;
#ifdef MINEH_OPENGL
                case Renderer::Type::GL: shader = new GL::Shader((GL::Context*)context); break;
#endif
#ifdef MINEH_VULKAN
                case Renderer::Type::Vk: shader = new Vk::Shader((Vk::Context*)context); break;
#endif
            }
            shader->main = main;
            shader->stage = stage;
            if (path.length() || subpath.length())
            {
                if (subpath.length())
                    switch (context->type) {
                        default: break;
#ifdef MINEH_OPENGL
                        case Renderer::Type::GL: shader->loadFromFile(path + "/OpenGL/" + subpath); break;
#endif
#ifdef MINEH_VULKAN
                        case Renderer::Type::Vk: shader->loadFromFile(path + "/Vulkan/spv/" + subpath); break;
#endif
                    }
                else shader->loadFromFile(path);
            }
            return shader;
        }
        
        mh::Descriptor* Descriptor(Renderer* context)
        {
            mh::Descriptor* descriptor = nullptr;
            switch (context->type)
            {
                default: return nullptr; break;
#ifdef MINEH_OPENGL
                case Renderer::Type::GL: descriptor = new GL::Descriptor((GL::Context*)context); break;
#endif
#ifdef MINEH_VULKAN
                case Renderer::Type::Vk: descriptor = new Vk::Descriptor((Vk::Context*)context); break;
#endif
            }
            return descriptor;
        }
        
        mh::Pipeline* Pipeline(Renderer* context)
        {
            mh::Pipeline* pipeline = nullptr;
            switch (context->type)
            {
                default: return nullptr; break;
#ifdef MINEH_OPENGL
                case Renderer::Type::GL: pipeline = new GL::Pipeline((GL::Context*)context); break;
#endif
#ifdef MINEH_VULKAN
                case Renderer::Type::Vk: pipeline = new Vk::Pipeline((Vk::Context*)context); break;
#endif
            }
            return pipeline;
        }
    }
}

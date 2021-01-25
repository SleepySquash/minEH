//
//  Image.cpp
//  OpenGL App
//
//  Created by Никита Исаенко on 19.01.2021.
//

#include "Texture.hpp"

#include <stb_image.h>

#include "../../Support/ResourcePath.hpp"

namespace mh
{
    TextureCollectorObject::TextureCollectorObject(const uint32_t& usage) : usage(usage) { }
    TextureCollectorObject::~TextureCollectorObject()
    {
        // if (savePixels) stbi_image_free(pixels);
    }
    std::unordered_map<std::string, TextureCollectorObject> TextureCollector::map;
    TextureCollector::Context TextureCollector::context;
    Renderer::Type TextureCollector::type{ Renderer::Type::undef };
    
#ifdef MINEH_OPENGL
    void TextureCollector::bindContext(GL::Context* context) { TextureCollector::context.gl = context; type = Renderer::Type::GL; }
#endif
#ifdef MINEH_VULKAN
    void TextureCollector::bindContext(Vk::Context* context) { TextureCollector::context.vk = context; type = Renderer::Type::Vk; }
#endif
    void* TextureCollector::get(const std::string& image, const Filter& samplerFilter)
    {
        auto it = map.find(image);
        if (it != map.end())
        {
            ++(*it).second.usage;
            return (*it).second.texture;
        }
        else
        {
            it = map.emplace(image, 1).first;
            switch (type)
            {
                default: throw std::runtime_error("TextureCollector :: get :: No context is bind! You need to tc::bindContext(*context)!"); break;
#ifdef MINEH_VULKAN
                case Renderer::Type::Vk:
                {
                    Vk::Texture* texture = new Vk::Texture();
                    VkFilter filter;
                    switch (samplerFilter)
                    {
                        default: filter = VK_FILTER_LINEAR; break;
                        case Filter::LINEAR: filter = VK_FILTER_LINEAR; break;
                        case Filter::NEAREST: filter = VK_FILTER_NEAREST; break;
                        case Filter::CUBIC: filter = VK_FILTER_CUBIC_IMG; break;
                    }
                    *texture = context.vk->generateTexture(resourcePath() + image, 0, filter);
                    (*it).second.texture = texture;
                } break;
#endif
#ifdef MINEH_OPENGL
                case Renderer::Type::GL:
                {
                    GL::Texture* texture = new GL::Texture();
                    GLenum filter;
                    switch (samplerFilter)
                    {
                        default: filter = GL_LINEAR; break;
                        case Filter::LINEAR: filter = GL_LINEAR; break;
                        case Filter::NEAREST: filter = GL_NEAREST; break;
                    }
                    *texture = context.gl->loadTexture(resourcePath() + image, filter);
                    (*it).second.texture = texture;
                } break;
#endif
            }
            return (*it).second.texture;
        }
    }

    void TextureCollector::erase(const std::string& image, const uint32_t& count)
    {
        auto it = map.find(image);
        if (it != map.end())
        {
            if ((*it).second.usage == 1 || (*it).second.usage == 0)
            {
                if ((*it).second.destroyable)
                {
                    switch (type)
                    {
                        default: throw std::runtime_error("TextureCollector :: erase :: No context is bind! You need to tc::bindContext(*context)!"); break;
#ifdef MINEH_VULKAN
                        case Renderer::Type::Vk:
                        {
                            Vk::Texture* texture = (Vk::Texture*)((*it).second.texture);
                            context.vk->freeTexture(*texture);
                            delete texture;
                        } break;
#endif
#ifdef MINEH_OPENGL
                        case Renderer::Type::GL:
                        {
                            GL::Texture* texture = (GL::Texture*)((*it).second.texture);
                            if (texture->id) glDeleteTextures(1, &texture->id);
                            delete texture;
                        } break;
#endif
                    }
                    map.erase(it);
                }
                else (*it).second.usage = 0;
            }
            else --(*it).second.usage;
        }
    }
    
    void TextureCollector::clear()
    {
        for (auto it = map.begin(); it != map.end(); ++it)
        {
            switch (type)
            {
                default: break;
#ifdef MINEH_VULKAN
                case Renderer::Type::Vk:
                {
                    Vk::Texture* texture = (Vk::Texture*)((*it).second.texture);
                    context.vk->freeTexture(*texture);
                    delete texture;
                } break;
#endif
#ifdef MINEH_OPENGL
                case Renderer::Type::GL:
                {
                    GL::Texture* texture = (GL::Texture*)((*it).second.texture);
                    if (texture->id) glDeleteTextures(1, &texture->id);
                    delete texture;
                } break;
#endif
            }
        }
        map.clear();
    }
}

//
//  Font.cpp
//  minEH
//
//  Created by Никита Исаенко on 05.02.2021.
//

#include "Font.hpp"
#include <vector>

#include "../../Support/ResourcePath.hpp"
#ifdef MINEH_OPENGL
    #include "../../Renderer/OpenGL/Text.hpp"
#endif

#ifdef MINEH_VULKAN
    #include <vulkan/vulkan.h>
#endif

namespace mh
{
    FontCollectorObject::FontCollectorObject(const uint32_t& usage) : usage(usage) { }
    FontCollectorObject::~FontCollectorObject()
    {
        // if (savePixels) stbi_image_free(pixels);
    }
    std::unordered_map<std::string, FontCollectorObject> FontCollector::map;
    FontCollector::Context FontCollector::context;
    Renderer::Type FontCollector::type{ Renderer::Type::undef };
    uint32_t FontCollector::ids = 1;
    
#ifdef MINEH_OPENGL
    void FontCollector::bindContext(GL::Context* context) { FontCollector::context.gl = context; type = Renderer::Type::GL; }
#endif
#ifdef MINEH_VULKAN
    void FontCollector::bindContext(Vk::Context* context) { FontCollector::context.vk = context; type = Renderer::Type::Vk; }
#endif
    FontCollectorObject* FontCollector::get(const std::string& id)
    {
        auto it = map.find(id);
        if (it != map.end())
        {
            ++(*it).second.usage;
            return &(*it).second;
        }
        else
        {
            switch (type)
            {
                default: break;
#ifdef MINEH_OPENGL
                case Renderer::Type::GL:
                {
                    auto it = map.emplace(id, 1).first;
                    GL::GLFont* font = new GL::GLFont(context.gl);
                    font->open(id); // TODO: if failed then load standart font instead
                    (*it).second.font = (void*)font;
                    return &(*it).second;
                } break;
#endif
#ifdef MINEH_VULKAN
                /*case Renderer::Type::Vk:
                {
                    std::vector<Vertex<glm::vec2>> vector = {
                        { { -1.f,  -1.f }, { 0.0f,  0.0f } },
                        { {  1.f,  -1.f }, { 1.0f,  0.0f } },
                        { { -1.f,   1.f }, { 0.0f,  1.0f } },
                        { {  1.f,  -1.f }, { 1.0f,  0.0f } },
                        { {  1.f,   1.f }, { 1.0f,  1.0f } },
                        { { -1.f,   1.f }, { 0.0f,  1.0f } } };
                    auto it = map.emplace(id, 1).first;
                    Vk::Buffer* buffer = new Vk::Buffer;
                    (*it).second.buffer = (void*)buffer;
                    (*it).second.id = ids++;
                    
                    context.vk->generateSingleBuffer(sizeof(vector[0]) * vector.size(), *buffer, vector.data(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
                    
                    return &(*it).second;
                } break;*/
#endif
            }
            
            return nullptr;
        }
    }
    FontCollectorObject* FontCollector::raw(const std::string& id)
    {
        if (map.find(id) == map.end()) return nullptr;
        else
        {
            FontCollectorObject* fco = get(id);
            --fco->usage; return fco;
        }
    }
    
    bool FontCollector::exists(const std::string& id) { return (map.find(id) != map.end()); }

    void FontCollector::erase(const std::string& id, const uint32_t& count)
    {
        auto it = map.find(id);
        if (it != map.end())
        {
            if ((*it).second.usage == 1 || (*it).second.usage == 0)
            {
                if ((*it).second.destroyable)
                {
                    switch (type)
                    {
                        default: break;
#ifdef MINEH_VULKAN
                        /*case Renderer::Type::Vk:
                        {
                            Vk::Buffer* buffer = (Vk::Buffer*)((*it).second.buffer);
                            context.vk->freeBuffer(*buffer);
                            delete buffer;
                        } break;*/
#endif
#ifdef MINEH_OPENGL
                        case Renderer::Type::GL:
                        {
                            GL::GLFont* font = (GL::GLFont*)((*it).second.font);
                            font->close();
                            delete font;
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
    
    void FontCollector::clear()
    {
        for (auto it = map.begin(); it != map.end(); ++it)
        {
            switch (type)
            {
                default: break;
#ifdef MINEH_VULKAN
                /*case Renderer::Type::Vk:
                {
                    Vk::Buffer* buffer = (Vk::Buffer*)((*it).second.buffer);
                    context.vk->freeBuffer(*buffer);
                    delete buffer;
                } break;*/
#endif
#ifdef MINEH_OPENGL
                case Renderer::Type::GL:
                {
                    GL::GLFont* font = (GL::GLFont*)((*it).second.font);
                    font->close();
                    delete font;
                } break;
#endif
            }
        }
        map.clear(); ids = 1;
    }
}

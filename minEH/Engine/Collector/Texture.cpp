//
//  Image.cpp
//  OpenGL App
//
//  Created by Никита Исаенко on 19.01.2021.
//

#include "Texture.hpp"

#include <stb_image.h>
#include <iostream>

#include "../../Support/ResourcePath.hpp"
#include "../../Renderer/Factory.hpp"

namespace mh
{
    TextureCollectorObject::TextureCollectorObject(const uint32_t& usage) : usage(usage) { }
    std::unordered_map<std::string, TextureCollectorObject*> TextureCollector::map;
    std::list<std::unordered_map<std::string, TextureCollectorObject*>::iterator> TextureCollector::trash;
    Renderer* TextureCollector::context = nullptr;
    
    void TextureCollector::bindContext(Renderer* context) { TextureCollector::context = context; }
    TextureCollectorObject* TextureCollector::get(const std::string& path, const TextureFilter& samplerFilter)
    {
        auto it = map.find(path);
        if (it != map.end()) {
            ++(*it).second->usage;
            if ((*it).second->frames) {
                (*it).second->frames = 0;
                for (auto iter = trash.begin(); iter != trash.end(); ++iter)
                    if (*iter == it) { trash.erase(iter); break; }
            }
            return (*it).second;
        } else {
            it = map.emplace(path, nullptr).first;
            (*it).second = new TextureCollectorObject(1);
            
            Image image(resourcePath() + path);
            Texture* texture = Renders::Texture(context);
            texture->loadFromImage(&image, samplerFilter);
            (*it).second->texture = texture;
            
            return (*it).second;
        }
    }
    TextureCollectorObject* TextureCollector::raw(const std::string& image)
    {
        if (map.find(image) == map.end()) return nullptr;
        TextureCollectorObject* tco = get(image);
        --tco->usage; return tco;
    }

    void TextureCollector::erase(const std::string& image, const uint32_t& count)
    {
        auto it = map.find(image);
        if (it != map.end())
        {
            if ((*it).second->usage == 1 || (*it).second->usage == 0)
            {
                if ((*it).second->destroyable)
                {
                    switch (context->type)
                    {
                        default: (*it).second->frames = 1; break;
#ifdef MINEH_VULKAN
                        case Renderer::Type::Vk: (*it).second->frames = ((Vk::Context*)context)->commandBuffers.size() + 1; break;
#endif
                    }
                    trash.push_back(it);
                }
                else (*it).second->usage = 0;
            }
            else --(*it).second->usage;
        }
    }
    
    void TextureCollector::frame()
    {
        for (auto it = trash.begin(); it != trash.end();)
        {
            --((**it).second->frames);
            if ((**it).second->frames == 0)
            {
                ((**it).second->texture)->free();
                delete ((**it).second->texture);
                delete (**it).second;
                map.erase(*it);
                it = trash.erase(it);
            }
            else ++it;
        }
    }
    
    void TextureCollector::clear()
    {
        for (auto it = map.begin(); it != map.end(); ++it)
        {
            ((*it).second->texture)->free();
            delete ((*it).second->texture);
            delete (*it).second;
        }
        map.clear();
        trash.clear();
    }
}

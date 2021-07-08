//
//  Font.cpp
//  minEH
//
//  Created by Никита Исаенко on 05.02.2021.
//

#include "Font.hpp"
#include <vector>

#include "../../Support/ResourcePath.hpp"

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
            }
        }
        map.clear(); ids = 1;
    }
}

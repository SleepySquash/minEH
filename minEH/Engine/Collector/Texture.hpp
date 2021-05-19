//
//  Image.hpp
//  OpenGL App
//
//  Created by Никита Исаенко on 19.01.2021.
//

#pragma once

#include <unordered_map>
#include <list>

#include "../../Support/Config.hpp"
#include "../../Renderer/Context.hpp"

namespace mh
{
    struct TextureCollectorObject
    {
        Texture* texture = nullptr;
        uint32_t usage;
        bool destroyable = true;
        uint8_t frames;
        TextureCollectorObject(const uint32_t& usage = 0);
    };

    struct Context;
    struct TextureCollector
    {
        static Renderer* context;
        static std::unordered_map<std::string, TextureCollectorObject*> map;
        static std::list<std::unordered_map<std::string, TextureCollectorObject*>::iterator> trash;
        
        static void bindContext(Renderer* context);
        static TextureCollectorObject* get(const std::string& image, const TextureFilter& samplerFilter = TextureFilter::LINEAR);
        static TextureCollectorObject* raw(const std::string& image);
        static void erase(const std::string& image, const uint32_t& count = 1);
        static void frame();
        static void clear();
    };

    typedef TextureCollector tc;
}

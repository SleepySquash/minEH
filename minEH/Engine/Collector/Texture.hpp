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
        void* texture = nullptr;
        uint32_t usage;
        bool destroyable = true;
        uint8_t frames;
        
        TextureCollectorObject(const uint32_t& usage = 0);
        ~TextureCollectorObject();
    };

    struct Context;
    struct TextureCollector
    {
        union Context {
#ifdef MINEH_OPENGL
            GL::Context* gl;
#endif
#ifdef MINEH_VULKAN
            Vk::Context* vk;
#endif
        };
        static Context context;
        static Renderer::Type type;
        static std::unordered_map<std::string, TextureCollectorObject*> map;
        static std::list<std::unordered_map<std::string, TextureCollectorObject*>::iterator> trash;
        
        enum class Filter { LINEAR, NEAREST, CUBIC };
        
#ifdef MINEH_OPENGL
        static void bindContext(GL::Context* context);
#endif
#ifdef MINEH_VULKAN
        static void bindContext(Vk::Context* context);
#endif
        static TextureCollectorObject* get(const std::string& image, const Filter& samplerFilter = Filter::LINEAR);
        static TextureCollectorObject* raw(const std::string& image);
        static void erase(const std::string& image, const uint32_t& count = 1);
        static void frame();
        static void clear();
    };

    typedef TextureCollector tc;
}

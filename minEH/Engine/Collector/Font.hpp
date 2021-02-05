//
//  Font.hpp
//  minEH
//
//  Created by Никита Исаенко on 05.02.2021.
//

#pragma once

#include <iostream>
#include <unordered_map>

#include "../../Support/Config.hpp"
#include "../../Renderer/Context.hpp"

namespace mh
{
    struct FontCollectorObject
    {
        void* font = nullptr;
        uint32_t usage, id;
        bool destroyable = true;
        
        FontCollectorObject(const uint32_t& usage = 0);
        ~FontCollectorObject();
    };

    struct Context;
    struct FontCollector
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
        static std::unordered_map<std::string, FontCollectorObject> map;
        static uint32_t ids;
        
#ifdef MINEH_OPENGL
        static void bindContext(GL::Context* context);
#endif
#ifdef MINEH_VULKAN
        static void bindContext(Vk::Context* context);
#endif
        static FontCollectorObject* get(const std::string& id);
        static FontCollectorObject* raw(const std::string& id);
        static bool exists(const std::string& id);
        static void erase(const std::string& id, const uint32_t& count = 1);
        static void clear();
    };

    typedef FontCollector fc;
}

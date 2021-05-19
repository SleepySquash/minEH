//
//  Buffer.hpp
//  OpenGL App
//
//  Created by Никита Исаенко on 20.01.2021.
//

#pragma once

#include <iostream>
#include <unordered_map>

#include "../../Support/Config.hpp"
#include "../../Renderer/Context.hpp"

namespace mh
{
    struct BufferCollectorObject
    {
        Buffer* buffer = nullptr;
        uint32_t usage, id, indices;
        bool destroyable = true;
        BufferCollectorObject(const uint32_t& usage = 0);
    };

    struct Context;
    struct BufferCollector
    {
        static Renderer* context;
        static std::unordered_map<std::string, BufferCollectorObject> map;
        static uint32_t ids;
        
        static void bindContext(Renderer* context);
        static BufferCollectorObject* get(const std::string& id);
        static BufferCollectorObject* raw(const std::string& id);
        static bool exists(const std::string& id);
        static void erase(const std::string& id, const uint32_t& count = 1);
        static void clear();
    };

    typedef BufferCollector bc;
}

//
//  Composition.hpp
//  OpenGL App
//
//  Created by Никита Исаенко on 13.01.2021.
//

#pragma once

#include <iostream>
#include <list>

#include "../Window/Window.hpp"
#include "../Renderer/Context.hpp"

using std::cout;
using std::endl;

namespace mh
{
    namespace GL { struct Context; }
    struct Component
    {
        virtual void onInit();
        virtual void onDestroy();
        virtual void onUpdate(const float& elapsed);
        virtual void onEvent(Event& event);
        virtual void onRecord(const uint32_t& i);
        virtual void onDraw();
    };
    
    struct Entity
    {
        bool dead{ false };
        std::list<Component*> components;
        
        template <typename T, typename ...Args> T* addComponent(Args... args)
        {
            T* component = new T(args...);
            components.push_back(component);
            component->onInit();
            return component;
        }
        
        void die();
        void destroy();
        void onUpdate(const float& elapsed);
        void onEvent(Event& event);
        void onRecord(const uint32_t& i);
        void onDraw();
    };
    
    struct Composition
    {
        std::list<Entity*> entities;
        
        Entity* addEntity();
        
        void destroy();
        void update(const float& elapsed);
        void event(Event& event);
        void record(const uint32_t& i);
        void draw();
    };
}

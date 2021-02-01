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
    struct Entity;
    struct Component
    {
        bool dead{ false };
        Entity* entity;
        
        virtual void onInit();
        virtual void onDestroy();
        virtual void onUpdate(const float& elapsed);
        virtual void onEvent(Event& event);
        virtual void onRecord(const uint32_t& i);
        virtual void onDraw();
        
        void doResize();
        void die();
    };
    
    struct Composition;
    struct Entity
    {
        bool dead{ false };
        Composition* composition;
        std::list<Component*> components;
        
        template <typename T, typename ...Args> T* addComponent(Args... args)
        {
            T* component = new T(args...);
            component->entity = this;
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
        Renderer* context;
        
        Composition(Renderer* context = nullptr);
        void bindContext(Renderer* context);
        
        Entity* addEntity();
        
        void destroy();
        void update(const float& elapsed);
        void event(Event& event);
        void record(const uint32_t& i);
        void draw();
    };
}

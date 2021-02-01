//
//  Composition.cpp
//  OpenGL App
//
//  Created by Никита Исаенко on 13.01.2021.
//

#include "Composition.hpp"

namespace mh
{
    void Component::onInit() { }
    void Component::onDestroy() { }
    void Component::onUpdate(const float& elapsed) { }
    void Component::onEvent(Event& event) { }
    void Component::onRecord(const uint32_t& i) { }
    void Component::onDraw() { }
    void Component::die() { dead = true; }
    void Component::doResize() { Event event; event.type = Event::Type::Resized; event.data.size = entity->composition->context->window->frame; onEvent(event); }

    void Entity::die() { dead = true; }
    void Entity::destroy()
    {
        for (auto it = components.begin(); it != components.end();)
        {
            (*it)->onDestroy(); delete (*it);
            it = components.erase(it);
        }
    }
    void Entity::onUpdate(const float& elapsed) {
        for (auto it = components.begin(); it != components.end();) {
            if ((*it)->dead) { (*it)->onDestroy(); delete (*it); it = components.erase(it); } else (*it++)->onUpdate(elapsed); } }
    void Entity::onEvent(Event& event) { for (auto& c : components) if (!c->dead) c->onEvent(event); }
    void Entity::onRecord(const uint32_t& i) { for (auto& c : components) if (!c->dead) c->onRecord(i); }
    void Entity::onDraw() { for (auto& c : components) if (!c->dead) c->onDraw(); }

    Composition::Composition(Renderer* context) : context(context) { }
    void Composition::bindContext(Renderer* context) { this->context = context; }
    Entity* Composition::addEntity()
    {
        Entity* entity = new Entity();
        entity->composition = this;
        entities.push_back(entity);
        return entity;
    }
    void Composition::destroy() { for (auto& e : entities) e->destroy(); }
    void Composition::update(const float& elapsed) {
        for (auto it = entities.begin(); it != entities.end();) {
            if ((*it)->dead) { (*it)->destroy(); delete (*it); it = entities.erase(it); } else (*it++)->onUpdate(elapsed); } }
    void Composition::event(Event& event) { for (auto& e : entities) e->onEvent(event); }
    void Composition::record(const uint32_t& i) { for (auto& e : entities) e->onRecord(i); }
    void Composition::draw() { for (auto& e : entities) e->onDraw(); }
}

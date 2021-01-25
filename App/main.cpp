//
//  main.cpp
//  OpenGL App
//
//  Created by Никита Исаенко on 14.01.2021.
//

#include <iostream>
using std::cout;
using std::endl;
#include <thread>
#include <chrono>

#include "../minEH/minEH.hpp"
using namespace mh;

#include "Components/Test.hpp"

void _main(int, const char**)
{
    cout << "main()\n";
    
    Window window;
    window.open();
    
    Vk::Context context(&window);
    
    Composition composition;
    Entity* entity = composition.addEntity();
    for (int i = 0; i < 100; ++i) { auto *npc = entity->addComponent<test::NPC>(&context, "Images/Vanilla.png"); npc->sprite->setScale({0.3f, 0.3f}); }
    
    Camera* camera = Graphics::GetCamera(&context);
    context.camera = camera;
    
    Entity* player = composition.addEntity();
    player->addComponent<FreeCamera>(&window, camera);
    
    {
        Entity* entity = composition.addEntity();
        auto* mesh = entity->addComponent<test::MeshTest>(&context, "Models/chalet.obj", "Models/chalet.jpg");
        mesh->mesh->setRotation({glm::radians(-90.f), 0.f, 0.f});
        mesh->mesh->setPosition({0.f, 0.f, 0.f});
    }
    
    {
        Entity* entity = composition.addEntity();
        auto* mesh = entity->addComponent<test::MeshTest>(&context);
        mesh->mesh->setScale({0.5f, 0.5f, 0.5f});
        mesh->mesh->setPosition({1.f, -1.f, -2.f});
    }
    
    Event event;
    Clock clock;
    while (window.isOpen)
    {
        if (window.pollEvent(event))
        {
            switch (event.type)
            {
                case Event::Type::Closed: window.close(); break;
                case Event::Type::Resized: window.resize(event); context.resize(); break;
                default: break;
            }
            composition.event(event);
        }
        
        composition.update(clock.restart());
        
        Clock framerate;
        {
            uint32_t i = context.beginDraw();
            
            context.beginRecord(i);
            composition.record(i);
            context.endRecord(i);
            
            composition.draw();
            
            context.endDraw(i);
            window.display();
        }
        cout << std::to_string(framerate.restart() * 1000) << "\n"; // window.setTitle(std::to_string(framerate.restart() * 1000) + " ms");
    }
    
    context.wait();
    composition.destroy();
    context.destroy();
}

//
//  main.cpp
//  OpenGL App
//
//  Created by Никита Исаенко on 14.01.2021.
//

#include <iostream>
#include <thread>
#include <chrono>
using std::cout;
using std::endl;

#include "../minEH/minEH.hpp"
using namespace mh;

#pragma mark -
#pragma mark Forward declarations (since main() being at top is easier to navigate)

void demo_audio_song(int, const char**);
void demo_3d_2d_scene(int, const char**);
void demo_keys(int, const char**);

#pragma mark -
#pragma mark main() function

void _main(int argc, const char** argv)
{
    // Uncomment the demo you want to start
    // or the demos will be played one after another
    
    // demo_audio_song(argc, argv);
    // demo_3d_2d_scene(argc, argv);
    demo_keys(argc, argv);
}
























#pragma mark -
#pragma mark Demo - Audio song

void demo_audio_song(int, const char**)
{
    cout << "main()\n";
    Audio::Init();
    
    Audio music(resourcePath() + "Audio/Rascal Does Not Dream of Bunny Girl Senpai ED - Fukashigi no Carte COVER 不可思議のカルテ 歌ってみた.ogg");
    if (music.isOpen)
    {
        Audio sound(resourcePath() + "Audio/476588__kurireeva__anime-cat-girl.wav"); sound.play();
        
        music.setPosition(50);
        // music.setLooped(true);
        music.play();
        while (music.isPlaying())
        {
            std::cout << music.getPosition() << "/" << music.duration << "\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
    
    Audio::Destroy();
}

#pragma mark -
#pragma mark Demo - 3D & 2D scene

#include "Components/Test.hpp"
void demo_3d_2d_scene(int, const char**)
{
    cout << "main()\n";
    Audio::Init();
    
    Window window;
    window.open();
    
    Vk::Context context(&window);
    
    Composition composition(&context);
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
    
    Audio music(resourcePath() + "Audio/Rascal Does Not Dream of Bunny Girl Senpai ED - Fukashigi no Carte COVER 不可思議のカルテ 歌ってみた.ogg");
    if (music.isOpen)
    {
        music.setPosition(0);
        music.setLooped(true);
        music.play();
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
    Audio::Destroy();
}

#pragma mark -
#pragma mark Demo - Keys game

#include "Components/Keys.hpp"
void demo_keys(int, const char**)
{
    cout << "main()\n";
    Audio::Init();
    
    Window window;
    window.open();
    
    GL::Context context(&window);
    
    Composition composition(&context);
    Entity* entity = composition.addEntity();
    entity->addComponent<keys::Game>(resourcePath() + "Keymaps/Marina Rios - Fukashigi no Carte/easy.kmp");
    
    Entity* debugEntity = composition.addEntity();
    debugEntity->addComponent<Components::Debug>();
    
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
                case Event::Type::KeyPressed:
                    switch (event.data.key)
                    {
                        default: break;
                        case Keyboard::Key::Num0: entity->addComponent<keys::Game>(resourcePath() + "Keymaps/Ken Ashcorp - I'm Your Slave/easy.kmp")->doResize(); break;
                    }
                    break;
                default: break;
            }
            composition.event(event);
        }
        
        composition.update(clock.restart());
        
        {
            uint32_t i = context.beginDraw();
            
            context.beginRecord(i);
            composition.record(i);
            context.endRecord(i);
            
            composition.draw();
            
            context.endDraw(i);
            window.display();
        }
    }
    
    context.wait();
    composition.destroy();
    context.destroy();
    Audio::Destroy();
}


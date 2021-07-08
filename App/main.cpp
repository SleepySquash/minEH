//
//  main.cpp
//  OpenGL App
//
//  Created by Никита Исаенко on 14.01.2021.
//

#include <iostream>
#include <thread>
#include <chrono>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
using std::cout;
using std::endl;

#include "../minEH/minEH.hpp"
using namespace mh;

#include "Examples/Examples.hpp"

#pragma mark -
#pragma mark Forward declarations (since main() being at top is easier to navigate)

void demo_audio_song(int, const char**);
void _indevelopment();

#pragma mark -
#pragma mark main() function

void _main(int argc, const char** argv)
{
    // Uncomment the demo you want to start
    // or the demos will be played one after another
    std::cout << "_main()\n";
    
    // Examples::Basics::AnimatedImage();
    _indevelopment();
    // demo_audio_song(argc, argv);
}




void _indevelopment()
{
    Window window;
    window.open();
    
    Vk::Context context(&window);
    Composition composition(&context);
    
    Texture* texture = Renders::Texture(&context,
                                        resourcePath() + "Images/nekos/dbcf0cbc94c8b5e3f649f770a7cbb57649a42cc0_hq2.jpg");
    RawSprite sprite(&context, texture);
    sprite.setScale(2.f);
    RawSprite sprite1(&context, texture);
    
    uint32_t frame = 0;
    float elapsed = 0.f;
    
    Event event;
    Clock clock;
    while (window.isOpen)
    {
        if (window.pollEvent(event))
        {
            switch (event.type)
            {
                case Event::Type::Closed: window.close(); break;
                case Event::Type::Resized: window.resize(event); context.resize();
                    sprite.resize(); sprite.setPosition(glm::vec2(window.width / 2, window.height / 2));
                    sprite1.resize();
                    break;
                default: break;
            }
            composition.event(event);
        }
        
        if (elapsed >= 0.07) {
            uint32_t framex = frame % 9, framey = frame / 9;
            float w = texture->width / 9.f, h = texture->height / 5.f;
            sprite.setUV(glm::vec4(framex * w, framey * h, w, h));
            ++frame; if (frame >= 45) frame = 0;
            elapsed = 0.f;
            
            window.shouldRender = true;
        }
        else elapsed += clock.elapsed();
        
        float elapsedSeconds = clock.restart();
        if (elapsedSeconds <= 0.01) std::this_thread::sleep_for(std::chrono::milliseconds(10));
        composition.update(elapsedSeconds);
        
        Clock framerate;
        if (window.shouldRender)
        {
            uint32_t i = context.beginDraw();
            {
                context.beginRecord(i);
                {
                    sprite.record(i);
                    // sprite1.record(i);
                    composition.record(i);
                }
                context.endRecord(i);
                composition.draw();
            }
            context.endDraw(i);
            window.display();
        }
        // cout << std::to_string(framerate.restart() * 1000) << "\n"; // window.setTitle(std::to_string(framerate.restart() * 1000) + " ms");
    }
    
    context.wait();
    composition.destroy();
    sprite.destroy();
    sprite1.destroy();
    texture->free();
    context.destroy();
}
























#pragma mark -
#pragma mark Demo - Audio song

void demo_audio_song(int, const char**)
{
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

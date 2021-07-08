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
using std::cout;
using std::endl;

#include "../minEH/minEH.hpp"
using namespace mh;

#pragma mark -
#pragma mark Forward declarations (since main() being at top is easier to navigate)

void demo_audio_song(int, const char**);
void demo_2d_scene(int, const char**);
void demo_3d_2d_scene(int, const char**);
void demo_keys(int, const char**);
void _indevelopment();

#pragma mark -
#pragma mark main() function

void _main(int argc, const char** argv)
{
    // Uncomment the demo you want to start
    // or the demos will be played one after another
    
    std::cout << "_main()\n";
    _indevelopment();
    // demo_audio_song(argc, argv);
    // demo_2d_scene(argc, argv);
    // demo_3d_2d_scene(argc, argv);
    // demo_keys(argc, argv);
}




void _indevelopment()
{
    Window window;
    window.open();
    
    Vk::Context context(&window);
    Composition composition(&context);
    
    
    float vertices[] = { -1.f, -1.f,   1.f, -1.f,   -1.f, 1.f,   1.f, 1.f };
    float uvs[] = { 0.0f, 0.0f,  1.0f, 0.0f,  0.0f, 1.0f,  1.0f, 1.0f };
    uint32_t indices[] = { 0, 1, 2,   1, 3, 2 };
    
    Buffer* vertexBuffer = Renders::Buffer(&context, BufferType::Vertex, vertices, sizeof(vertices));
    Buffer* uvBuffer = Renders::Buffer(&context, BufferType::Vertex, uvs, sizeof(uvs), BufferUpdateType::Dynamic);
    Buffer* indexBuffer = Renders::Buffer(&context, BufferType::Index, indices, sizeof(indices));
    
    float combined[] = { -1.f, -1.f, 0.0f, 0.0f,   1.f, -1.f, 1.0f, 0.0f,   -1.f, 1.f, 0.0f, 1.0f,   1.f, 1.f, 1.0f, 1.0f  };
    Buffer* combinedBuffer = Renders::Buffer(&context, BufferType::Vertex, combined, sizeof(combined));
    
    
    Texture* texture = Renders::Texture(&context, resourcePath() + "Images/nekos/dbcf0cbc94c8b5e3f649f770a7cbb57649a42cc0_hq2.jpg");
    Descriptor* descriptor = Renders::Descriptor(&context);
    descriptor->layouts = {
        { DescriptorType::CombinedImageSampler, 0, ShaderStage::Fragment, texture, nullptr }
    };
    descriptor->allocate();
    
    
    Shader *vertexShader = Renders::Shader(&context, ShaderStage::Vertex, resourcePath() + "Shaders", "sprite.vert"),
           *fragmentShader = Renders::Shader(&context, ShaderStage::Fragment, resourcePath() + "Shaders", "sprite.frag");
    Pipeline* pipeline = Renders::Pipeline(&context);
    pipeline->shaders = { vertexShader, fragmentShader };
    pipeline->bindings = {
        { 0, sizeof(float) * 2 },
        { 1, sizeof(float) * 2 }
    };
    pipeline->attributes = {
        // { 0, 0, 0, VertexFormat::R32G32_SFLOAT },
        // { 1, 0, 8, VertexFormat::R32G32_SFLOAT }
        { 0, 0, 0, VertexFormat::R32G32_SFLOAT },
        { 1, 1, 0, VertexFormat::R32G32_SFLOAT },
    };
    pipeline->pushConstantRanges = {
        {   ShaderStage::Vertex, "model",                 0, sizeof(glm::mat4) },
        { ShaderStage::Fragment, "color", sizeof(glm::mat4), sizeof(glm::vec4) },
    };
    pipeline->descriptor = descriptor;
    pipeline->allocate();
    

    glm::vec4 color = glm::vec4(1.f);
    glm::mat4 model = glm::mat4(1.f);
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
                case Event::Type::Resized: window.resize(event); context.resize(); break;
                default: break;
            }
            composition.event(event);
        }
        
        // model = glm::rotate(model, 0.1f * elapsed, glm::vec3(0.f, 0.f, 1.f));
        if (elapsed >= 0.07) {
            uint32_t framex = frame % 9, framey = frame / 9;
            float w = 1.0 / 9.f, h = 1.0 / 5.f;
            float uv[] = {
                (framex + 0) * w, (framey + context.UV_0) * h,
                (framex + 1) * w, (framey + context.UV_0) * h,
                (framex + 0) * w, (framey + context.UV_1) * h,
                (framex + 1) * w, (framey + context.UV_1) * h };
            uvBuffer->update(uv, sizeof(uv));
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
                    pipeline->onRecord(i);          // once per Shader
                    pipeline->vertex({ vertexBuffer, uvBuffer });      // once per Object
                    pipeline->index(indexBuffer);       // once per Object
                    descriptor->onRecord(pipeline);        // once per Object
                    pipeline->push(0, &model);      // every    Object
                    pipeline->push(1, &color);      // every    Object
                    pipeline->drawIndexed(6, 6); // every    Object
                    
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
    vertexBuffer->free();
    uvBuffer->free();
    indexBuffer->free();
    combinedBuffer->free();
    vertexShader->free();
    fragmentShader->free();
    texture->free();
    descriptor->free();
    pipeline->free();
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

#pragma mark -
#pragma mark Demo - 2D scene

#include "Components/Test.hpp"
void demo_2d_scene(int, const char**)
{
    Window window;
    window.open();
    
    GL::Context context(&window);
    
    Composition composition(&context);
    Entity* entity = composition.addEntity();
    std::string nekos[] = { "Vanilla", "Chocola", "Azuki", "Coconut", "Cinnamon", "Shigure" };
    for (int i = 0; i < 10; ++i) {
        auto *npc = entity->addComponent<test::NPC>("Images/nekos/" + nekos[rand() % 6] + ".png");
        npc->sprite->setScale({0.3f, 0.3f});
    }
    
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
                case Event::Type::TouchBegan: std::cout << "Touch began (" << event.data.touch.finger << "): " << event.data.touch.x << " " << event.data.touch.y << "\n"; break;
                case Event::Type::TouchMoved: std::cout << "Touch moved (" << event.data.touch.finger << "): " << event.data.touch.x << " " << event.data.touch.y << "\n"; break;
                case Event::Type::TouchEnded: std::cout << "Touch ended (" << event.data.touch.finger << "): " << event.data.touch.x << " " << event.data.touch.y << "\n"; break;
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

#pragma mark -
#pragma mark Demo - 3D & 2D scene

#include "Components/Test.hpp"
void demo_3d_2d_scene(int, const char**)
{
    Audio::Init();
    
    Window window;
    window.open();
    
    Vk::Context context(&window);
    
    Composition composition(&context);
    Entity* entity = composition.addEntity();
    for (int i = 0; i < 100; ++i) { auto *npc = entity->addComponent<test::NPC>("Images/Vanilla.png"); npc->sprite->setScale({0.3f, 0.3f}); }
    
    Camera* camera = Graphics::GetCamera(&context);
    context.camera = camera;
    
    Entity* player = composition.addEntity();
    player->addComponent<FreeCamera>(&window, camera);
    
    {
        Entity* entity = composition.addEntity();
        auto* mesh = entity->addComponent<test::MeshTest>("Models/chalet.obj", "Models/chalet.jpg");
        mesh->mesh->setRotation({glm::radians(-90.f), 0.f, 0.f});
        mesh->mesh->setPosition({0.f, 0.f, 0.f});
    }
    
    {
        Entity* entity = composition.addEntity();
        auto* mesh = entity->addComponent<test::MeshTest>();
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
    Audio::Init();
    
    Window window;
    window.open();
    
    Vk::Context context(&window);
    
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


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
    
    GL::Context context(&window);
    Composition composition(&context);
    
    
    float vertices[] = { -1.f, -1.f,   1.f, -1.f,   -1.f, 1.f,   1.f, 1.f };
    float uvs[] = { 0.0f, 0.0f,  1.0f, 0.0f,  0.0f, 1.0f,  1.0f, 1.0f };
    uint32_t indices[] = { 0, 1, 2,   1, 3, 2 };
    
    Buffer* vertexBuffer = Renders::Buffer(&context, BufferType::Vertex, vertices, sizeof(vertices));
    Buffer* uvBuffer = Renders::Buffer(&context, BufferType::UV, uvs, sizeof(uvs), BufferUpdateType::Dynamic);
    Buffer* indexBuffer = Renders::Buffer(&context, BufferType::Index, indices, sizeof(indices));
    
    
    Buffer* arrayBuffer = Renders::Buffer(&context, BufferType::Array);
    glBindVertexArray(((GL::Buffer*)arrayBuffer)->id);
    glBindBuffer(GL_ARRAY_BUFFER, ((GL::Buffer*)vertexBuffer)->id);
    glEnableVertexAttribArray(0); glVertexAttribPointer(0, 2, GL_FLOAT, false, 2 * sizeof(float), (void*)(0));
    glBindBuffer(GL_ARRAY_BUFFER, ((GL::Buffer*)uvBuffer)->id);
    glEnableVertexAttribArray(1); glVertexAttribPointer(1, 2, GL_FLOAT, false, 2 * sizeof(float), (void*)(0));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ((GL::Buffer*)indexBuffer)->id);
    glBindVertexArray(0);
    
    
    Shader *vertexShader = Renders::Shader(&context, ShaderStage::Vertex, resourcePath() + "Shaders/OpenGL/sprite.vert"),
           *fragmentShader = Renders::Shader(&context, ShaderStage::Fragment, resourcePath() + "Shaders/OpenGL/sprite.frag");
    Pipeline* pipeline = Renders::Pipeline(&context); // TODO: attributes and VAO belong to here
    pipeline->shaders = { vertexShader, fragmentShader };
    pipeline->allocate();
    
    GLuint mloc = glGetUniformLocation(pipeline->pipelineID, "model");
    GLuint cloc = glGetUniformLocation(pipeline->pipelineID, "color");
    glm::vec4 color = glm::vec4(1.f);
    
    
    Texture* texture = Renders::Texture(&context, resourcePath() + "Images/nekos/dbcf0cbc94c8b5e3f649f770a7cbb57649a42cc0_hq2.jpg");
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
        
        glm::mat4 model = glm::mat4(1.f);
        
        if (elapsed >= 0.07) {
            uint32_t framex = frame % 9, framey = frame / 9;
            float w = 1.0 / 9.f, h = 1.0 / 5.f;
            float uv[] = {
                (framex + 0) * w, (framey + 1) * h,
                (framex + 1) * w, (framey + 1) * h,
                (framex + 0) * w, (framey + 0) * h,
                (framex + 1) * w, (framey + 0) * h };
            uvBuffer->update(uv, sizeof(uv));
            ++frame; if (frame >= 45) frame = 0;
            elapsed = 0.f;
            
            window.shouldRender = true;
        }
        else elapsed += clock.elapsed();
        
        composition.update(clock.restart());
        
        Clock framerate;
        if (window.shouldRender)
        {
            uint32_t i = context.beginDraw();
            
            context.beginRecord(i);
            composition.record(i);
            context.endRecord(i);
            
            pipeline->onRecord(i);
            glBindVertexArray(((GL::Buffer*)arrayBuffer)->id);
            glUniformMatrix4fv(mloc, 1, false, glm::value_ptr(model));
            glUniform4f(cloc, color.r, color.g, color.b, color.a);
            glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, ((GL::Texture*)texture)->id);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            
            composition.draw();
            
            context.endDraw(i);
            window.display();
        }
        if (clock.elapsed() <= 0.01) std::this_thread::sleep_for(std::chrono::milliseconds(10));
        // cout << std::to_string(framerate.restart() * 1000) << "\n"; // window.setTitle(std::to_string(framerate.restart() * 1000) + " ms");
    }
    
    context.wait();
    composition.destroy();
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


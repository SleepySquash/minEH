//
//  Sprite.cpp
//  minEH
//
//  Created by Никита Исаенко on 08.07.2021.
//

#include "../Examples.hpp"

#include <iostream>
#include <thread>
#include <chrono>
#include <glm/gtc/type_ptr.hpp>
using std::cout;
using std::endl;

#include "../../../minEH/minEH.hpp"

namespace Examples
{
    namespace Basics
    {
        void AnimatedImage()
        {
            mh::Window window;
            window.open();
            
            mh::Vk::Context context(&window);
            
            float vertices[] = { -1.f, -1.f,   1.f, -1.f,   -1.f, 1.f,   1.f, 1.f };
            float uvs[] = { 0.0f, 0.0f,  1.0f, 0.0f,  0.0f, 1.0f,  1.0f, 1.0f };
            uint32_t indices[] = { 0, 1, 2,   1, 3, 2 };
            
            mh::Buffer* vertexBuffer = mh::Renders::Buffer(&context, mh::BufferType::Vertex, vertices, sizeof(vertices));
            mh::Buffer* uvBuffer = mh::Renders::Buffer(&context, mh::BufferType::Vertex, uvs, sizeof(uvs), mh::BufferUpdateType::Dynamic);
            mh::Buffer* indexBuffer = mh::Renders::Buffer(&context, mh::BufferType::Index, indices, sizeof(indices));
            
            mh::Texture* texture = mh::Renders::Texture(&context, mh::resourcePath() + "Images/nekos/dbcf0cbc94c8b5e3f649f770a7cbb57649a42cc0_hq2.jpg");
            mh::Descriptor* descriptor = mh::Renders::Descriptor(&context);
            descriptor->layouts = { { mh::DescriptorType::CombinedImageSampler, 0, mh::ShaderStage::Fragment } };
            descriptor->allocate();
            descriptor->update({ texture });
            
            mh::Shader *vertexShader = mh::Renders::Shader(&context, mh::ShaderStage::Vertex, mh::resourcePath() + "Shaders", "sprite.vert"),
                       *fragmentShader = mh::Renders::Shader(&context, mh::ShaderStage::Fragment, mh::resourcePath() + "Shaders", "sprite.frag");
            mh::Pipeline* pipeline = mh::Renders::Pipeline(&context);
            pipeline->shaders = { vertexShader, fragmentShader };
            pipeline->bindings = {
                { 0, sizeof(float) * 2 },
                { 1, sizeof(float) * 2 }
            };
            pipeline->attributes = {
                { 0, 0, 0, mh::VertexFormat::R32G32_SFLOAT },
                { 1, 1, 0, mh::VertexFormat::R32G32_SFLOAT },
            };
            pipeline->pushConstantRanges = {
                {   mh::ShaderStage::Vertex, "model",                 0, sizeof(glm::mat4) },
                { mh::ShaderStage::Fragment, "color", sizeof(glm::mat4), sizeof(glm::vec4) },
            };
            pipeline->descriptor = descriptor;
            pipeline->allocate();

            glm::vec4 color = glm::vec4(1.f);
            glm::mat4 model = glm::mat4(1.f);
            uint32_t frame = 0;
            float elapsed = 0.f;
            
            mh::Event event;
            mh::Clock clock;
            while (window.isOpen)
            {
                if (window.pollEvent(event))
                {
                    switch (event.type)
                    {
                        case mh::Event::Type::Closed: window.close(); break;
                        case mh::Event::Type::Resized: window.resize(event); context.resize(); pipeline->recreate(); break;
                        default: break;
                    }
                }
                
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
                        }
                        context.endRecord(i);
                    }
                    context.endDraw(i);
                    window.display();
                }
            }
            
            context.wait();
            
            vertexBuffer->free();
            uvBuffer->free();
            indexBuffer->free();
            vertexShader->free();
            fragmentShader->free();
            texture->free();
            descriptor->free();
            pipeline->free();
            
            context.destroy();
        }
    }
}

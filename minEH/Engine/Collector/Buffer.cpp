//
//  Buffer.cpp
//  OpenGL App
//
//  Created by Никита Исаенко on 20.01.2021.
//

#include "Buffer.hpp"
#include <vector>

#ifdef MINEH_OPENGL
    #include <OpenGL/gl3.h>
#endif

#ifdef MINEH_VULKAN
    #include <vulkan/vulkan.h>
#endif

namespace mh
{
    BufferCollectorObject::BufferCollectorObject(const uint32_t& usage) : usage(usage) { }
    BufferCollectorObject::~BufferCollectorObject()
    {
        // if (savePixels) stbi_image_free(pixels);
    }
    std::unordered_map<std::string, BufferCollectorObject> BufferCollector::map;
    BufferCollector::Context BufferCollector::context;
    Renderer::Type BufferCollector::type{ Renderer::Type::undef };
    uint32_t BufferCollector::ids = 1;
    
#ifdef MINEH_OPENGL
    void bc::bindContext(GL::Context* context) { bc::context.gl = context; type = Renderer::Type::GL; }
#endif
#ifdef MINEH_VULKAN
    void bc::bindContext(Vk::Context* context) { bc::context.vk = context; type = Renderer::Type::Vk; }
#endif
    BufferCollectorObject* BufferCollector::get(const std::string& id)
    {
        auto it = map.find(id);
        if (it != map.end())
        {
            ++(*it).second.usage;
            return &(*it).second;
        }
        else
        {
            if (id == "quad")
            {
                switch (type)
                {
                    default: break;
#ifdef MINEH_OPENGL
                    case Renderer::Type::GL:
                    {
                        GLfloat vector[] = {
                            -1.f,  1.f,    0.0f, 1.0f,
                            -1.f, -1.f,    0.0f, 0.0f,
                             1.f, -1.f,    1.0f, 0.0f,
                            -1.f,  1.f,    0.0f, 1.0f,
                             1.f, -1.f,    1.0f, 0.0f,
                             1.f,  1.f,    1.0f, 1.0f,
                        };
                        
                        auto it = map.emplace(id, 1).first;
                        GL::Buffer* buffer = new GL::Buffer;
                        (*it).second.buffer = (void*)buffer;
                        
                        glGenVertexArrays(1, &buffer->VAO);
                        glBindVertexArray(buffer->VAO);
                        glGenBuffers(1, &buffer->VBO);
                        glBindBuffer(GL_ARRAY_BUFFER, buffer->VBO);
                        glBufferData(GL_ARRAY_BUFFER, sizeof(vector), vector, GL_STATIC_DRAW);
                        glEnableVertexAttribArray(0); glVertexAttribPointer(0, 2, GL_FLOAT, false, 4 * sizeof(float), (void*)(0));
                        glEnableVertexAttribArray(1); glVertexAttribPointer(1, 2, GL_FLOAT, false, 4 * sizeof(float), (void*)(2 * sizeof(float)));
                        glBindVertexArray(0);
                        
                        return &(*it).second;
                    } break;
#endif
#ifdef MINEH_VULKAN
                    case Renderer::Type::Vk:
                    {
                        std::vector<Vertex<glm::vec2>> vector = {
                            { { -1.f,  -1.f }, { 0.0f,  0.0f } },
                            { {  1.f,  -1.f }, { 1.0f,  0.0f } },
                            { { -1.f,   1.f }, { 0.0f,  1.0f } },
                            { {  1.f,  -1.f }, { 1.0f,  0.0f } },
                            { {  1.f,   1.f }, { 1.0f,  1.0f } },
                            { { -1.f,   1.f }, { 0.0f,  1.0f } } };
                        auto it = map.emplace(id, 1).first;
                        Vk::Buffer* buffer = new Vk::Buffer;
                        (*it).second.buffer = (void*)buffer;
                        (*it).second.id = ids++;
                        
                        context.vk->generateSingleBuffer(sizeof(vector[0]) * vector.size(), *buffer, vector.data(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
                        
                        return &(*it).second;
                    } break;
#endif
                }
            }
            else if (id == "quad-v")
            {
                switch (type)
                {
                    default: break;
#ifdef MINEH_VULKAN
                    case Renderer::Type::Vk:
                    {
                        std::vector<Vertex<glm::vec2>> vector = {
                            { { -1.f,  -1.f }, { 0.0f,  0.0f } },
                            { {  1.f,  -1.f }, { 1.0f,  0.0f } },
                            { { -1.f,   1.f }, { 0.0f,  1.0f } },
                            { {  1.f,   1.f }, { 1.0f,  1.0f } } };
                        auto it = map.emplace(id, 1).first;
                        Vk::Buffer* buffer = new Vk::Buffer;
                        (*it).second.buffer = (void*)buffer;
                        (*it).second.id = ids++;
                        
                        context.vk->generateSingleBuffer(sizeof(vector[0]) * vector.size(), *buffer, vector.data(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
                        
                        return &(*it).second;
                    } break;
#endif
                }
            }
            else if (id == "quad-i")
            {
                switch (type)
                {
                    default: break;
#ifdef MINEH_VULKAN
                    case Renderer::Type::Vk:
                    {
                        std::vector<uint32_t> vector = { 0, 1, 2,   1, 3, 2 };
                        auto it = map.emplace(id, 1).first;
                        Vk::Buffer* buffer = new Vk::Buffer;
                        (*it).second.buffer = (void*)buffer;
                        (*it).second.id = ids++;
                        
                        context.vk->generateSingleBuffer(sizeof(vector[0]) * vector.size(), *buffer, vector.data(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
                        
                        return &(*it).second;
                    } break;
#endif
                }
            }
            else
            {
                auto it = map.emplace(id, 1).first;
                (*it).second.id = ids++;
                return &(*it).second;
            }
            
            return nullptr;
        }
    }
    
    bool BufferCollector::exists(const std::string& id) { return (map.find(id) != map.end()); }

    void BufferCollector::erase(const std::string& id, const uint32_t& count)
    {
        auto it = map.find(id);
        if (it != map.end())
        {
            if ((*it).second.usage == 1 || (*it).second.usage == 0)
            {
                if ((*it).second.destroyable)
                {
                    switch (type)
                    {
                        default: break;
#ifdef MINEH_VULKAN
                        case Renderer::Type::Vk:
                        {
                            Vk::Buffer* buffer = (Vk::Buffer*)((*it).second.buffer);
                            context.vk->freeBuffer(*buffer);
                            delete buffer;
                        } break;
#endif
#ifdef MINEH_OPENGL
                        case Renderer::Type::GL:
                        {
                            GL::Buffer* buffer = (GL::Buffer*)((*it).second.buffer);
                            if (buffer->VAO) glDeleteVertexArrays(1, &buffer->VAO);
                            if (buffer->VAO) glDeleteBuffers(1, &buffer->VAO);
                            if (buffer->EBO) glDeleteBuffers(1, &buffer->EBO);
                            delete buffer;
                        } break;
#endif
                    }
                    map.erase(it);
                }
                else (*it).second.usage = 0;
            }
            else --(*it).second.usage;
        }
    }
    
    void BufferCollector::clear()
    {
        for (auto it = map.begin(); it != map.end(); ++it)
        {
            switch (type)
            {
                default: break;
#ifdef MINEH_VULKAN
                case Renderer::Type::Vk:
                {
                    Vk::Buffer* buffer = (Vk::Buffer*)((*it).second.buffer);
                    context.vk->freeBuffer(*buffer);
                    delete buffer;
                } break;
#endif
#ifdef MINEH_OPENGL
                case Renderer::Type::GL:
                {
                    GL::Buffer* buffer = (GL::Buffer*)((*it).second.buffer);
                    if (buffer->VAO) glDeleteVertexArrays(1, &buffer->VAO);
                    if (buffer->VAO) glDeleteBuffers(1, &buffer->VAO);
                    if (buffer->EBO) glDeleteBuffers(1, &buffer->EBO);
                    delete buffer;
                } break;
#endif
            }
        }
        map.clear(); ids = 1;
    }
}

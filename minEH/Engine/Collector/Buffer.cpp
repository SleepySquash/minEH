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

#include "../../Renderer/Factory.hpp"

namespace mh
{
    BufferCollectorObject::BufferCollectorObject(const uint32_t& usage) : usage(usage) { }
    std::unordered_map<std::string, BufferCollectorObject> BufferCollector::map;
    Renderer* BufferCollector::context = nullptr;
    uint32_t BufferCollector::ids = 1;
    
    void bc::bindContext(Renderer* context) { bc::context = context; }
    BufferCollectorObject* BufferCollector::get(const std::string& id)
    {
        auto it = map.find(id);
        if (it != map.end()) { ++(*it).second.usage; return &(*it).second; }
        else
        {
            auto it = map.emplace(id, 1).first;
            Buffer* buffer = Renders::Buffer(context);
            (*it).second.buffer = buffer;
            (*it).second.id = ids++;
            
            if (id == "quad") {
                float vector[] = {
                    -1.f,  1.f,    0.0f, 1.0f,
                    -1.f, -1.f,    0.0f, 0.0f,
                     1.f, -1.f,    1.0f, 0.0f,
                    -1.f,  1.f,    0.0f, 1.0f,
                     1.f, -1.f,    1.0f, 0.0f,
                     1.f,  1.f,    1.0f, 1.0f,
                }; buffer->allocate(vector, sizeof(vector));
            } else if (id == "quad-v") {
                float vector[] = { -1.f, -1.f,   1.f, -1.f,   -1.f, 1.f,   1.f, 1.f };
                buffer->allocate(vector, sizeof(vector));
            } else if (id == "quad-i") {
                float vector[] = { 0, 1, 2,   1, 3, 2 };
                buffer->allocate(vector, sizeof(vector));
            }
            
            return &(*it).second;
        }
    }
    BufferCollectorObject* BufferCollector::raw(const std::string& id) {
        if (map.find(id) == map.end()) return nullptr;
        else { BufferCollectorObject* bco = get(id); --bco->usage; return bco; }
    }
    bool BufferCollector::exists(const std::string& id) { return (map.find(id) != map.end()); }
    void BufferCollector::erase(const std::string& id, const uint32_t& count)
    {
        auto it = map.find(id);
        if (it != map.end()) {
            if ((*it).second.usage == 1 || (*it).second.usage == 0) {
                if ((*it).second.destroyable) {
                    ((*it).second.buffer)->free();
                    delete ((*it).second.buffer);
                    map.erase(it);
                } else (*it).second.usage = 0;
            } else --(*it).second.usage;
        }
    }
    void BufferCollector::clear()
    {
        for (auto it = map.begin(); it != map.end(); ++it) { ((*it).second.buffer)->free(); delete ((*it).second.buffer); }
        map.clear(); ids = 1;
    }
    
        /*        switch (type)
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
                        
                        context.gl->VAOID = context.gl->VBOID = context.gl->EBOID = 0;
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
            else if (id == "text")
            {
                switch (type)
                {
                    default: break;
#ifdef MINEH_OPENGL
                    case Renderer::Type::GL:
                    {
                        GLfloat vector[] = {
                             0.f,  1.f,    0.0f, 0.0f,
                             0.f,  0.f,    0.0f, 1.0f,
                             1.f,  0.f,    1.0f, 1.0f,
                             0.f,  1.f,    0.0f, 0.0f,
                             1.f,  0.f,    1.0f, 1.0f,
                             1.f,  1.f,    1.0f, 0.0f,
                        };
                        
                        auto it = map.emplace(id, 1).first;
                        GL::Buffer* buffer = new GL::Buffer;
                        (*it).second.buffer = (void*)buffer;
                        
                        glGenVertexArrays(1, &buffer->VAO);
                        glBindVertexArray(buffer->VAO);
                        glGenBuffers(1, &buffer->VBO);
                        glBindBuffer(GL_ARRAY_BUFFER, buffer->VBO);
                        glBufferData(GL_ARRAY_BUFFER, sizeof(vector), vector, GL_STATIC_DRAW);
                        glEnableVertexAttribArray(0); glVertexAttribPointer(0, 4, GL_FLOAT, false, 4 * sizeof(float), (void*)(0));
                        glBindVertexArray(0);
                        
                        context.gl->VAOID = context.gl->VBOID = 0;
                        return &(*it).second;
                    } break;
#endif
#ifdef MINEH_VULKAN
                    case Renderer::Type::Vk:
                    {
                        std::vector<Vertex<glm::vec2>> vector = {
                            { {  0.f,   0.f }, { 0.0f,  0.0f } },
                            { {  1.f,   0.f }, { 1.0f,  0.0f } },
                            { {  0.f,   1.f }, { 0.0f,  1.0f } },
                            { {  1.f,   0.f }, { 1.0f,  0.0f } },
                            { {  1.f,   1.f }, { 1.0f,  1.0f } },
                            { {  0.f,   1.f }, { 0.0f,  1.0f } } };
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
#pragma mark -
#pragma mark quad-v
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
            else if (id == "sprite") {
                switch (type) {
                    default: break;
#ifdef MINEH_OPENGL
                    case Renderer::Type::GL:
                    {
                        GLfloat vertices[] = { -1.f, -1.f,   1.f, -1.f,   -1.f, 1.f,   1.f, 1.f };
                        GLfloat uvs[] = { 0.0f, 0.0f,  1.0f, 0.0f,  0.0f, 1.0f,  1.0f, 1.0f };
                        uint32_t indices[] = { 0, 1, 2,   1, 3, 2 };
                        
                        auto it = map.emplace(id, 1).first;
                        GL::Buffer* buffer = new GL::Buffer;
                        (*it).second.buffer = (void*)buffer;
                        
                        glGenVertexArrays(1, &buffer->VAO);
                        glGenBuffers(1, &buffer->VBO);
                        glGenBuffers(1, &buffer->UVO);
                        glGenBuffers(1, &buffer->EBO);
                        
                        glBindVertexArray(buffer->VAO);
                        
                        glBindBuffer(GL_ARRAY_BUFFER, buffer->VBO);
                        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
                        glEnableVertexAttribArray(0); glVertexAttribPointer(0, 2, GL_FLOAT, false, 2 * sizeof(float), (void*)(0));
                        
                        glBindBuffer(GL_ARRAY_BUFFER, buffer->UVO);
                        glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), uvs, GL_STATIC_DRAW);
                        glEnableVertexAttribArray(1); glVertexAttribPointer(1, 2, GL_FLOAT, false, 2 * sizeof(float), (void*)(0));
                        
                        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->EBO);
                        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
                        
                        glBindVertexArray(0);
                        
                        context.gl->VAOID = context.gl->VBOID = context.gl->UVOID = context.gl->EBOID = 0;
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
         */
}

//
//  OpenGL.cpp
//  OpenGL App
//
//  Created by Никита Исаенко on 21.01.2021.
//

#include "../../Support/Config.hpp"
#ifdef MINEH_OPENGL

#include <stb_image.h>
#include <iostream>

#include "OpenGL.hpp"
#include "../../Engine/Collector/Texture.hpp"
#include "../../Engine/Collector/Buffer.hpp"
#include "../../Engine/Collector/Shader.hpp"
#include "../../Engine/Collector/Font.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace mh
{
    namespace GL
    {
#pragma mark -
#pragma mark Buffer
        
        Buffer::Buffer(Renderer* context) : context((GL::Context*)context) { }
        void Buffer::allocate(void* data, uint32_t size)
        {
            switch (type) {
                default: glGenBuffers(1, &id); break;
                case BufferType::Array: glGenVertexArrays(1, &id); break;
            }
            if (data) update(data, size);
        }
        void Buffer::update(void* data, uint32_t size) {
            switch (type) {
                default:
                    glBindBuffer(GL_ARRAY_BUFFER, id);
                    glBufferData(GL_ARRAY_BUFFER, size, data, updateType == BufferUpdateType::Static ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
                    break;
                case BufferType::Array: break;
            }
        }
        void Buffer::free() {
            switch (type) {
                default: glDeleteBuffers(1, &id); break;
                case BufferType::Array: glDeleteVertexArrays(1, &id); break;
            }
        }
        
#pragma mark -
#pragma mark Texture
        
        Texture::Texture(Renderer* context) : context((GL::Context*)context) { }
        void Texture::allocate(void* data, uint32_t width, uint32_t height, TextureFilter filter)
        {
            glGenTextures(1, &id);
            glBindTexture(GL_TEXTURE_2D, id);
            
            GLenum _filter;
            switch (filter)
            {
                default: _filter = GL_LINEAR; break;
                case TextureFilter::LINEAR: _filter = GL_LINEAR; break;
                case TextureFilter::NEAREST: _filter = GL_NEAREST; break;
            }
            
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _filter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _filter);
            
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        void Texture::free() { glDeleteTextures(1, &id); id = 0; }
        
#pragma mark -
#pragma mark ...
        
        Shader::Shader(Renderer* context) : context((GL::Context*)context) { }
        bool Shader::loadFromFile(const std::string& path)
        {
            std::ifstream file(path);
            if (file.is_open())
            {
                std::stringstream stream;
                stream << file.rdbuf();
                std::string code = stream.str();
                const char* _code = code.c_str();
                
                int ec; char ecode[512];
                
                switch (stage) { default: break;
                    case ShaderStage::Vertex: id = glCreateShader(GL_VERTEX_SHADER); break;
                    case ShaderStage::Fragment: id = glCreateShader(GL_FRAGMENT_SHADER); break;
                    case ShaderStage::Geometry: id = glCreateShader(GL_GEOMETRY_SHADER); break;
                }
                
                glShaderSource(id, 1, &_code, NULL);
                glCompileShader(id);
                glGetShaderiv(id, GL_COMPILE_STATUS, &ec);
                if (!ec)
                {
                    glGetShaderInfoLog(id, 512, NULL, ecode);
                    std::cout << "glCompileShader(" << path << ") Error: " << ecode << "\n";
                }
                else return true;
            }
            return false;
        }
        void Shader::free() { glDeleteShader(id); }
        
        
        Descriptor::Descriptor(Renderer* context) : context((GL::Context*)context) { }
        void Descriptor::allocate() { }
        void Descriptor::free() { }
        void Descriptor::onRecord(mh::Pipeline* pipeline)
        {
            GLuint tex = GL_TEXTURE0;
            for (auto& d : layouts) {
                if (d.type == DescriptorType::CombinedImageSampler) {
                    glActiveTexture(tex++);
                    glBindTexture(GL_TEXTURE_2D, ((GL::Texture*)d.texture)->id);
                }
            }
        }
        
        
        Pipeline::Pipeline(Renderer* context) : context((GL::Context*)context) { }
        void Pipeline::allocate()
        {
            pipelineID = glCreateProgram();
            for (auto& s : shaders) glAttachShader(pipelineID, ((GL::Shader*)s)->id);
            glLinkProgram(pipelineID);
            
            int ec; char ecode[512];
            glGetProgramiv(pipelineID, GL_LINK_STATUS, &ec);
            if (!ec) {
                glGetProgramInfoLog(pipelineID, 512, NULL, ecode);
                std::cout << "glLinkProgram() Error(s):\n" << ecode << "\n";
                return;
            }
            
            glGenVertexArrays(1, &VAO);
            /*glGenVertexArrays(1, &VAO);
            glBindVertexArray(VAO);
            
            for (auto& a : attributes) {
                switch (a.buffer->type) { default: break;
                    case BufferType::Index: glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ((GL::Buffer*)a.buffer)->id); continue; break;
                    case BufferType::Vertex: glBindBuffer(GL_ARRAY_BUFFER, ((GL::Buffer*)a.buffer)->id); break;
                    case BufferType::UV: glBindBuffer(GL_ARRAY_BUFFER, ((GL::Buffer*)a.buffer)->id); break; }
                
                int format; uint32_t count, size;
                switch (a.format) { default: format = GL_FLOAT; count = 0; size = 0; break;
                    case VertexFormat::R32G32_SFLOAT: count = 2; format = GL_FLOAT; size = 4 * 2; break;
                    case VertexFormat::R32G32B32_SFLOAT: count = 3; format = GL_FLOAT; size = 4 * 3; break;
                    case VertexFormat::R32G32B32A32_SFLOAT: count = 4; format = GL_FLOAT; size = 4 * 4; break; }
                uint32_t stride = 0; for (auto& b : bindings) { if (b.binding == a.binding) { stride = b.stride; break; } }
                glEnableVertexAttribArray(a.location); glVertexAttribPointer(a.location, count, GL_FLOAT, false, stride, (void*)(a.offset));
                std::cout << a.location << " " << count << " " << stride << " " << (void*)(a.offset) << '\n';
            }
            glBindVertexArray(0);*/
        }
        void Pipeline::free() { glDeleteProgram(pipelineID); glDeleteVertexArrays(1, &VAO); }
        void Pipeline::onRecord(const uint32_t& i) {
            if (context->shaderID != pipelineID) { glUseProgram(pipelineID); context->shaderID = pipelineID; }
            if (context->VAOID != VAO) { glBindVertexArray(VAO); context->VAOID = VAO; }
        }
        void Pipeline::vertex(const std::vector<mh::Buffer*>& buffers) {
            auto it = buffers.begin();
            for (auto& a : attributes) {
                if (a.location && !a.offset) ++it;
                glBindBuffer(GL_ARRAY_BUFFER, ((GL::Buffer*)(*it))->id);
                int format; uint32_t count, size;
                switch (a.format) { default: format = GL_FLOAT; count = 0; size = 0; break;
                    case VertexFormat::R32G32_SFLOAT: count = 2; format = GL_FLOAT; size = 4 * 2; break;
                    case VertexFormat::R32G32B32_SFLOAT: count = 3; format = GL_FLOAT; size = 4 * 3; break;
                    case VertexFormat::R32G32B32A32_SFLOAT: count = 4; format = GL_FLOAT; size = 4 * 4; break; }
                uint32_t stride = 0; for (auto& b : bindings) { if (b.binding == a.binding) { stride = b.stride; break; } }
                glEnableVertexAttribArray(a.location); glVertexAttribPointer(a.location, count, GL_FLOAT, false, stride, (void*)(a.offset));
            }
        }
        void Pipeline::index(mh::Buffer* buffer) { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ((GL::Buffer*)buffer)->id); }
        void Pipeline::push(const uint32_t& i, void* data)
        {
            switch (pushConstantRanges[i].size)
            {
                case sizeof(glm::vec4): {
                    glm::vec4* d = reinterpret_cast<glm::vec4*>(data);
                    glUniform4f(glGetUniformLocation(pipelineID, pushConstantRanges[i].name.c_str()), d->r, d->g, d->b, d->a);
                } break;
                case sizeof(glm::mat4): {
                    glm::mat4* d = reinterpret_cast<glm::mat4*>(data);
                    glUniformMatrix4fv(glGetUniformLocation(pipelineID, pushConstantRanges[i].name.c_str()), 1, false, glm::value_ptr(*d));
                } break;
                default: break;
            }
        }
        // TODO: GL_TRINAGLES from PipelineTopology - make function-converter
        void Pipeline::drawIndexed(const uint32_t& vertices, const uint32_t& indices) { glDrawElements(GL_TRIANGLES, vertices, GL_UNSIGNED_INT, 0); }
        
#pragma mark -
#pragma mark Context
        
        uint32_t Context::beginDraw(float r, float g, float b, float a)
        {
            glClearColor(r, g, b, a);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            return 0;
        }
        
        void Context::beginRecord(const uint32_t&) { shaderID = 0; }
        void Context::endRecord(const uint32_t&) { tc::frame(); }
        
        void Context::resize() { glViewport(0, 0, window->frame.width, window->frame.height); }
        void Context::destroy()
        {
            bc::clear();
            tc::clear();
            fc::clear();
        }
        void Context::wait() { }
        
        void Context::init()
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LEQUAL);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            
            UV_0 = 1;
            UV_1 = 0;
            
            bc::bindContext(this);
            tc::bindContext(this);
            fc::bindContext(this);
        }
    }
}

#endif

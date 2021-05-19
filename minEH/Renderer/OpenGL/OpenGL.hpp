//
//  OpenGL.h
//  minEH
//
//  Created by Никита Исаенко on 19.05.2021.
//

#pragma once
#include "../Renderer.hpp"

#ifdef MINEH_OPENGL
#include <OpenGL/OpenGL.h>
#include <OpenGL/gl3.h>

namespace mh
{
    namespace GL
    {
        // struct Buffer { GLuint VAO = 0, VBO = 0, UVO = 0, EBO = 0; };
        struct Context;
        struct Buffer : mh::Buffer
        {
            GL::Context* context;
            GLuint id;
            
            Buffer(Renderer* context);
            void allocate(void* data = nullptr, uint32_t size = 0) override;
            void update(void* data, uint32_t size) override;
            void free() override;
        };
        
        struct Image { uint32_t width, height; };
        struct Texture : mh::Texture
        {
            GL::Context* context;
            Image image;
            GLuint id;
            
            Texture(Renderer* context);
            void allocate(void* data, uint32_t width, uint32_t height, TextureFilter filter = TextureFilter::LINEAR) override;
            void free() override;
        };
        
        struct Shader : mh::Shader
        {
            GL::Context* context;
            GLuint id;
            
            Shader(Renderer* context);
            bool loadFromFile(const std::string& path) override;
            void free() override;
        };
        
        struct Pipeline : mh::Pipeline
        {
            GL::Context* context;
            
            Pipeline(Renderer* context);
            void allocate() override;
            void free() override;
            void onRecord(const uint32_t& i) override;
        };
        
        struct Context : Renderer
        {
            GLuint shaderID = 0, VAOID = 0, VBOID = 0, UVOID = 0, EBOID = 0, textureID = 0, CBOID = 0;
            
            Context();
            Context(Window* window);
            ~Context();
            
            void init();
            void create(Window* window);
            void setContext();
            void resize();
            void destroy();
            void wait();
            
            uint32_t beginDraw(float r = 0.f, float g = 0.f, float b = 0.f, float a = 1.f);
            void endDraw(const uint32_t& i = 0);
            
            void beginRecord(const uint32_t&);
            void endRecord(const uint32_t&);
            
    #if defined(MINEH_WIN32)
            HWND hwnd;
            HDC hdc;
            HGLRC hglrc;
    #elif defined(MINEH_MACOS) || defined(MINEH_IOS)
            void* view;
    #endif
        };
        
        void unsetContext(const Context& context);
        void destroyContext(const Context& context);
        bool getCurrentContext(const Context& context);
    }
}
#endif

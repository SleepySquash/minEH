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

#include "../Context.hpp"
#include "../../Engine/Collector/Texture.hpp"
#include "../../Engine/Collector/Buffer.hpp"
#include "../../Engine/Collector/Shader.hpp"

namespace mh
{
    namespace GL
    {
        Texture Context::loadTexture(const std::string& path, const GLenum& filter)
        {
            Texture texture;
            
            glGenTextures(1, &texture.id);
            glBindTexture(GL_TEXTURE_2D, texture.id);
            
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
            
            int width, height, channels;
            stbi_set_flip_vertically_on_load(true);
            unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
            if (data)
            {
                texture.image.width = static_cast<unsigned int>(width);
                texture.image.height = static_cast<unsigned int>(height);
                glTexImage2D(GL_TEXTURE_2D, 0, channels == 4 ? GL_RGBA : GL_RGB, width, height, 0, channels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);
            }
            else std::cout << "stbi_load() Error: failed to load texture '" << path << "'.\n";
            stbi_image_free(data);
            
            textureID = 0;
            return texture;
        }
        
        uint32_t Context::beginDraw(float r, float g, float b, float a)
        {
            glClearColor(r, g, b, a);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            return 0;
        }
        
        void Context::beginRecord(const uint32_t&) { }
        void Context::endRecord(const uint32_t&) { tc::frame(); }
        
        void Context::resize() { glViewport(0, 0, window->frame.width, window->frame.height); }
        void Context::destroy() { }
        void Context::wait() { }
        
        void Context::init()
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LEQUAL);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            
            bc::bindContext(this);
            tc::bindContext(this);
        }
    }
}

#endif

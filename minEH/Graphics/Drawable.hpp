//
//  Drawable.hpp
//  OpenGL App
//
//  Created by Никита Исаенко on 21.01.2021.
//

#pragma once

#include <string>
#include <glm/glm.hpp>
#include "../Engine/Modes.hpp"

namespace mh
{
    struct Drawable
    {
        virtual void record(const uint32_t& i);
        virtual void draw();
    };
    
    struct Transform2D
    {
        glm::vec2 position{ 0.f },
                  scale   { 1.f };
        float     rotation{ 0.f };
        glm::vec4 bounds;
        
        virtual void setPosition(const glm::vec2& pos);
        virtual void setRotation(const float&     rot);
        virtual void setScale   (const glm::vec2& scl);
        virtual void setScale   (const float&     scl);
        
        virtual const glm::vec2& getPosition();
        virtual const float&     getRotation();
        virtual const glm::vec2& getScale();
        
        virtual void dirty();
    };
    
    struct Sprite : Drawable, Transform2D
    {
        glm::vec4 color{ 1.f, 1.f, 1.f, 1.f };
        float xcoef{ 1.f }, ycoef{ 1.f };
        
        virtual void create();
        virtual void resize();
        virtual void destroy();
        
        virtual void setContext(void* context);
        virtual void setTexture(const std::string& path);
        virtual void setUV(const glm::vec4& uv); // TODO: vertex and UV buffers should be seperate vertex attributes (in) so that sprite can change independently their own UV buffer via glSubBufferData or vkCmdCopyBuffer (if UV change is frequent then it should use GL_DYNAMIC_DRAW and CPU visible buffer)
    };
    
    struct Font
    {
        virtual bool open(const std::string& path);
        virtual void close();
        
        virtual void setContext(void* context);
    };
    
    struct Text : Drawable, Transform2D
    {
        Align halign{ Align::Center }, valign{ Align::Middle };
        glm::vec4 color{ 1.f, 1.f, 1.f, 1.f };
        std::string string{ "Text" };
        
        virtual void create();
        virtual void resize();
        virtual void destroy();
        
        virtual void setContext(void* context);
        virtual void setFont(const std::string& path);
    };
    
    struct Transform3D
    {
        glm::vec3 position{ 0.f },
                  scale   { 1.f },
                  rotation{ 0.f };
        
        virtual void setPosition(const glm::vec3& pos);
        virtual void setRotation(const glm::vec3& rot);
        virtual void setScale   (const glm::vec3& scl);
        
        virtual const glm::vec3& getPosition();
        virtual const glm::vec3& getRotation();
        virtual const glm::vec3& getScale();
        
        virtual void dirty();
    };
    
    struct Mesh : Drawable, Transform3D
    {
        virtual void create();
        virtual void resize();
        virtual void destroy();
        
        virtual void setContext(void* context);
        virtual void setTexture(const std::string& path);
        virtual void setModel(const std::string& path);
    };
}

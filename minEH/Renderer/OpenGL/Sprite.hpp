//
//  Sprite.hpp
//  minEH
//
//  Created by Никита Исаенко on 23.01.2021.
//

#pragma once

#include "../../Support/Config.hpp"

#ifdef MINEH_OPENGL
#include <glm/glm.hpp>
#include "../Context.hpp"
#include "../../Graphics/Drawable.hpp"

namespace mh
{
    namespace GL
    {
        struct GLSprite : Sprite
        {
            std::pair<GL::Texture*, std::string> texture = { nullptr, "" };
            GL::Context* context = nullptr;
            
            glm::mat4 model;
            GL::Buffer *VAO, *VBO, *EBO, *UVO;
            GLuint ID, mloc, cloc;
            bool mDirty = true, mUV = false;
            
            GLSprite();
            GLSprite(GL::Context* context);
            
            void create() override;
            void destroy() override;
            void resize() override;
            
            void draw() override;
            void dirty() override;
            
            void setContext(void* context) override;
            void setTexture(const std::string& path) override;
            void setPosition(const glm::vec2& pos) override;
            void setUV(const glm::vec4 &uv) override;
        };
    }
}
#endif

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
            GLuint VAO, VBO, EBO, ID, loc;
            
            bool mDirty = true;
            float xcoef{ 1.f }, ycoef{ 1.f };
            
            GLSprite();
            GLSprite(GL::Context* context);
            
            void create() override;
            void destroy() override;
            void resize() override;
            
            void draw() override;
            void dirty() override;
            
            void setContext(void* context) override;
            void setTexture(const std::string& path) override;
        };
    }
}
#endif

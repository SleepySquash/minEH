//
//  Mesh.hpp
//  minEH
//
//  Created by Никита Исаенко on 25.01.2021.
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
        struct GLMesh : Mesh
        {
            std::pair<GL::Texture*, std::string> texture = { nullptr, "" };
            std::string modelPath;
            GL::Context* context = nullptr;
            
            glm::mat4 model;
            GLuint VAO, VBO, EBO, ID, loc;
            uint32_t indicesCount;
            
            bool mDirty = true;
            
            GLMesh(GL::Context* context = nullptr);
            
            void create() override;
            void destroy() override;
            void resize() override;
            
            void draw() override;
            void dirty() override;
            
            void setContext(void* context) override;
            void setTexture(const std::string& path) override;
            void setModel(const std::string& path) override;
        };
    }
}

#endif

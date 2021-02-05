//
//  Text.hpp
//  minEH
//
//  Created by Никита Исаенко on 02.02.2021.
//

#pragma once

#include "../../Support/Config.hpp"
#ifdef MINEH_OPENGL

#include <unordered_map>
#include <glm/glm.hpp>

#include "../../Graphics/Drawable.hpp"
#include "../Context.hpp"
#include "Shader.hpp"

namespace mh
{
    namespace GL
    {
        struct GLCharacter
        {
            GLuint     textureID;  // ID handle of the glyph texture
            glm::ivec2 size;       // Size of glyph
            glm::ivec2 bearing;    // Offset from baseline to left/top of glyph
            long       advance;    // Offset to advance to next glyph
        };
        
        struct GLFont : Font
        {
            Context* context;
            std::unordered_map<char, GLCharacter> characters; // TODO: char32_t
            
            GLFont();
            GLFont(Context* context);
            
            bool open(const std::string& path) override;
            void close() override;
            
            void setContext(void* context) override;
        };
        
        struct GLText : Text
        {
            std::pair<GLFont*, std::string> font;
            Context* context;
            
            glm::mat4 model;
            GLuint VAO, VBO, ID, mloc, cloc;
            bool mDirty = true;
            
            float xscale, yscale;
            float xbegin, ybegin;
            
            GLText();
            GLText(Context* context);
            
            void create() override;
            void destroy() override;
            void resize() override;
            
            void draw() override;
            void dirty() override;
            
            void setContext(void* context) override;
            void setFont(const std::string& path) override;
            void setPosition(const glm::vec2& pos) override;
        };
    }
}

#endif

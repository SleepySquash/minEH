//
//  RawText.hpp
//  minEH
//
//  Created by Никита Исаенко on 09.07.2021.
//

#pragma once

#include "../../../minEH/minEH.hpp"

namespace mh
{
    struct RawCharacter
    {
        Texture*   texture;    // ID handle of the glyph texture
        glm::ivec2 size;       // Size of glyph
        glm::ivec2 bearing;    // Offset from baseline to left/top of glyph
        long       advance;    // Offset to advance to next glyph
    };
    
    struct RawFont
    {
        std::unordered_map<char, RawCharacter> characters; // TODO: char32_t
        uint16_t characterSize;
        
        RawFont();
        RawFont(const std::string& path, const uint16_t& characterSize);
        bool open(const std::string& path, const uint16_t& characterSize);
        void free();
    };
    
    struct RawText : Transform2D, Drawable
    {
        Renderer* context;
        RawFont* font;
        
        RawText(Renderer* context);
        RawText(Renderer* context, RawFont* font);
        void create();
        void resize();
        void destroy();
        void record(const uint32_t& i) override;
        void setFont(RawFont* font);
    };
}

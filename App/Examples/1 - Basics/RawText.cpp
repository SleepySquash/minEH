//
//  RawText.cpp
//  minEH
//
//  Created by Никита Исаенко on 09.07.2021.
//

#include "RawText.hpp"

namespace mh
{
    RawFont::RawFont() { }
    RawFont::RawFont(const std::string& path, const uint16_t& characterSize) { open(path, characterSize); }
    bool RawFont::open(const std::string& path, const uint16_t& characterSize)
    {
        return false;
    }
    void RawFont::free() { }
    
    RawText::RawText(Renderer* context) : context(context) { }
    RawText::RawText(Renderer* context, RawFont* font) : context(context) { setFont(font); create(); }
    void RawText::create() { }
    void RawText::resize() { }
    void RawText::destroy() { }
    void RawText::record(const uint32_t& i) { }
    void RawText::setFont(RawFont* font) { this->font = font; }
}

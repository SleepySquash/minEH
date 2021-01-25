//
//  utf.cpp
//  OpenGL App
//
//  Created by Никита Исаенко on 19.01.2021.
//

#include "utf.hpp"
#include <codecvt>

namespace mh
{
    std::string utf8(const std::wstring& str)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
        return converter.to_bytes(str);
    }
    std::wstring utf16(const std::string& str)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
        return converter.from_bytes(str);
    }
}

//
//  utf.hpp
//  OpenGL App
//
//  Created by Никита Исаенко on 19.01.2021.
//

#pragma once

#include <iostream>

namespace mh
{
    std::string utf8(const std::wstring& str);
    std::wstring utf16(const std::string& str);
}

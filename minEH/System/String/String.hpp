//
//  String.hpp
//  minEH
//
//  Created by Никита Исаенко on 31.01.2021.
//

#pragma once

#include <iostream>

#include "../../Support/Config.hpp"

namespace mh
{
    struct String
    {
#ifdef MINEH_WIN32
        std::wstring string;
#else
        std::string string;
#endif
    };
}

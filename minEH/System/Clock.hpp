//
//  Clock.hpp
//  OpenGL App
//
//  Created by Никита Исаенко on 19.01.2021.
//

#pragma once

#include <chrono>

namespace mh
{
    struct Clock
    {
        std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
        
        Clock();
        float restart();
    };
}

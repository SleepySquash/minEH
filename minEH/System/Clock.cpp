//
//  Clock.cpp
//  OpenGL App
//
//  Created by Никита Исаенко on 19.01.2021.
//

#include "Clock.hpp"

namespace mh
{
    Clock::Clock() { startTime = std::chrono::high_resolution_clock::now(); }
    float Clock::restart()
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
        startTime = currentTime;
        
        return time;
    }
}

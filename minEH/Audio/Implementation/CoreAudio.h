//
//  CoreAudio.h
//  iOS App
//
//  Created by Никита Исаенко on 06.02.2021.
//

#pragma once

#include <mutex>

namespace mh
{
    struct AudioData
    {
        // SNDFILE *file = nullptr;
        // SF_INFO *sfinfo = nullptr;
        
        std::mutex mux;
        bool playing = false, looped = false, abort = false;
        // sf_count_t framesRead = 0;
    };
}

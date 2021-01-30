//
//  Audio.hpp
//  minEH
//
//  Created by Никита Исаенко on 27.01.2021.
//

#ifndef Audio_hpp
#define Audio_hpp

#include <iostream>
#include <sndfile.h>
#include <portaudio.h>

namespace mh
{
    struct MyAudioData
    {
        SNDFILE *file = nullptr;
        SF_INFO *sfinfo = nullptr;
        
        bool playing = false, looped = false;
        sf_count_t framesRead = 0;
    };
    
    struct Audio
    {
    private:
        MyAudioData d;
        PaStream *stream = nullptr;
        
    public:
        bool isOpen = false;
        double duration = 0;
        
        Audio();
        Audio(const std::string& filename);
        ~Audio();
        void open(const std::string& filename);
        void close();
        
        void play();
        void pause();
        void stop();
        
        bool isPlaying();
        bool isLooped();
        void setLooped(const bool& doLoop);
        double getPosition();
        void setPosition(const double& seconds);
        double getDuration();
        
        // Test audio system - this call plays "filename" audio file and waits for it to end playing
        void test(const std::string& filename);
        
        // Global audio system Init() and Destroy()
        static void Init();
        static void Destroy();
    };
}

#endif /* Audio_hpp */

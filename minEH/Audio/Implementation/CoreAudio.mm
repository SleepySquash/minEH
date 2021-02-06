//
//  CoreAudio.m
//  iOS App
//
//  Created by Никита Исаенко on 06.02.2021.
//

#import <Foundation/Foundation.h>
#include "CoreAudio.h"
#include "Audio.hpp"

namespace mh
{
    void Audio::Init()
    {
        std::cerr << "Error :: Audio :: CoreAudio for iOS is unavailable for now.\n";
    }
    void Audio::Destroy() {  }
    
    
    Audio::Audio() { d = new AudioData(); }
    Audio::Audio(const std::string& filename) { d = new AudioData(); open(filename); }
    Audio::~Audio()
    {
        delete d;
    }
    
    void Audio::open(const std::string& filename)
    {
        close();
    }
    void Audio::close()
    {
        
    }
    
    void Audio::play()
    {
        
    }
    
    void Audio::pause() { stop(); }
    void Audio::stop() {  }
    
    void Audio::setLooped(const bool& doLoop) { ((AudioData*)d)->looped = true; }
    bool Audio::isLooped() { return ((AudioData*)d)->looped; }
    bool Audio::isPlaying() { return ((AudioData*)d)->playing; }
    double Audio::getPosition() { return 0; }
    void Audio::setPosition(const double &seconds) {  }
    
    void Audio::test(const std::string& filename)
    {
        
    }
}

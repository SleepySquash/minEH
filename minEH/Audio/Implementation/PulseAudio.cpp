//
//  PulseAudio.cpp
//  macOS App
//
//  Created by Никита Исаенко on 06.02.2021.
//

#include "PulseAudio.hpp"
#include "Audio.hpp"

namespace mh
{
    void Audio::Init()
    {
        PaError err = Pa_Initialize();
        if (err != paNoError) std::cerr << "Error :: Audio :: " << Pa_GetErrorText( err ) << ".\n";
    }
    void Audio::Destroy() { Pa_Terminate(); }
    
    
    Audio::Audio() { d = new AudioData(); }
    Audio::Audio(const std::string& filename) { d = new AudioData(); open(filename); }
    Audio::~Audio()
    {
        ((AudioData*)d)->mux.lock();
        if (stream) Pa_AbortStream(stream);
        if (stream) Pa_CloseStream(stream);
        if (((AudioData*)d)->file) sf_close(((AudioData*)d)->file);
        if (((AudioData*)d)->sfinfo) delete ((AudioData*)d)->sfinfo;
        ((AudioData*)d)->mux.unlock();
        delete d;
    }
    
    void Audio::open(const std::string& filename)
    {
        close(); ((AudioData*)d)->sfinfo = new SF_INFO;
        if (!(((AudioData*)d)->file = sf_open(filename.c_str(), SFM_READ, ((AudioData*)d)->sfinfo))) {
            printf("Error :: Audio :: Failed to open file: \"%s\".\n", filename.c_str()); return; }
        duration = (double)((AudioData*)d)->sfinfo->frames / ((AudioData*)d)->sfinfo->samplerate; isOpen = true;
    }
    void Audio::close()
    {
        ((AudioData*)d)->mux.lock();
        if (stream) Pa_AbortStream(stream);
        if (stream) { Pa_CloseStream(stream); stream = nullptr; }
        if (((AudioData*)d)->file) { sf_close(((AudioData*)d)->file); ((AudioData*)d)->file = nullptr; }
        if (((AudioData*)d)->sfinfo) { delete ((AudioData*)d)->sfinfo; ((AudioData*)d)->sfinfo = nullptr; }
        ((AudioData*)d)->playing = false;
        ((AudioData*)d)->framesRead = 0;
        isOpen = false;
        ((AudioData*)d)->mux.unlock();
    }
    
    void Audio::play()
    {
        // Create PortAudio's Device and Stream
        PaStreamParameters outputParameters;
        outputParameters.device = Pa_GetDefaultOutputDevice();
        if (outputParameters.device == paNoDevice) { std::cerr << "Error :: Audio :: No default output device.\n"; return; }
        outputParameters.channelCount = ((AudioData*)d)->sfinfo->channels;
        outputParameters.sampleFormat = paFloat32;
        outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
        outputParameters.hostApiSpecificStreamInfo = NULL;
        
        PaError err = Pa_OpenStream(&stream, NULL, &outputParameters, ((AudioData*)d)->sfinfo->samplerate, 0, paClipOff,
                                    [](const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData) -> int
                                    {
                                        AudioData *d = (AudioData*)userData;
                                        d->mux.lock();
                                        if (!d->file || ! d->sfinfo || d->abort) { d->mux.unlock(); return paAbort; }
            
                                        float *out = (float*)outputBuffer;
                                        framesPerBuffer *= d->sfinfo->channels;
                                        
                                        sf_count_t readCount = 0;
                                        float data[framesPerBuffer];
                                        if ((readCount = sf_read_float(d->file, data, framesPerBuffer)))
                                        {
                                            // TODO: VOLUME    for (unsigned long i = 0; i < readCount; ++i) data[i] *= 4;
                                            if (d->looped && readCount != framesPerBuffer)
                                            {
                                                unsigned long left = framesPerBuffer - readCount;
                                                d->framesRead = 0; sf_seek(d->file, 0, SEEK_SET);
                                                readCount += sf_read_float(d->file, data + readCount, left);
                                            }
                                            memcpy(out, data, sizeof(float) * framesPerBuffer);
                                        }
                                        else if (d->looped)
                                        {
                                            d->framesRead = 0; sf_seek(d->file, 0, SEEK_SET);
                                            readCount = sf_read_float(d->file, data, framesPerBuffer);
                                        }
                                        else { d->mux.unlock(); return paComplete; }
                                        
                                        d->framesRead += readCount / d->sfinfo->channels;
            
                                        d->mux.unlock();
                                        return paContinue;
                                    }, &d);
        if (err != paNoError) { std::cerr << "Error :: Audio :: " << Pa_GetErrorText( err ) << ".\n"; return; }
        
        err = Pa_SetStreamFinishedCallback(stream, [](void* userData) { ((AudioData*)userData)->playing = false; });
        if (err != paNoError) std::cerr << "Error :: Audio :: " << Pa_GetErrorText( err ) << ".\n";
        err = Pa_StartStream(stream); if (err != paNoError) std::cerr << "Error :: Audio :: " << Pa_GetErrorText( err ) << ".\n"; else ((AudioData*)d)->playing = true;
    }
    
    void Audio::pause() { stop(); }
    void Audio::stop() { PaError err = Pa_StopStream(stream); if (err != paNoError) std::cerr << "Error :: Audio :: " << Pa_GetErrorText( err ) << ".\n"; }
    
    void Audio::setLooped(const bool& doLoop) { ((AudioData*)d)->looped = true; }
    bool Audio::isLooped() { return ((AudioData*)d)->looped; }
    bool Audio::isPlaying() { return ((AudioData*)d)->playing; }
    double Audio::getPosition() { return (double)((AudioData*)d)->framesRead / ((AudioData*)d)->sfinfo->samplerate; }
    void Audio::setPosition(const double &seconds) { ((AudioData*)d)->framesRead = (seconds * ((AudioData*)d)->sfinfo->samplerate); if (((AudioData*)d)->file) sf_seek(((AudioData*)d)->file, ((AudioData*)d)->framesRead, SEEK_SET); }
    
    void Audio::test(const std::string& filename)
    {
        // Open sndfile's Audio File
        AudioData d; d.sfinfo = new SF_INFO;
        
        if (!(d.file = sf_open(filename.c_str(), SFM_READ, d.sfinfo))) { printf ("Error : Not able to open input file %s.\n", filename.c_str()); return; }
        
        printf ("Sample Rate : %d\n", d.sfinfo->samplerate) ;
        if (d.sfinfo->frames == SF_COUNT_MAX) printf ("Frames      : unknown\n") ;
        else printf ("Frames      : %lld\n", d.sfinfo->frames) ;
        printf ("Channels    : %d\n", d.sfinfo->channels);
        printf ("Format      : 0x%08X\n", d.sfinfo->format);
        printf ("Sections    : %d\n", d.sfinfo->sections);
        printf ("Seekable    : %s\n", (d.sfinfo->seekable ? "TRUE" : "FALSE"));
        
        if (d.sfinfo->samplerate < 1) printf ("Duration    : NULL\n");
        if (d.sfinfo->frames / d.sfinfo->samplerate > 0x7FFFFFFF) printf ("Duration    : Unknown\n");
        printf ("Duration    : %f\n", (double)(1.0f * d.sfinfo->frames) / d.sfinfo->samplerate);
        
        // Create PortAudio's Device and Stream
        PaStreamParameters outputParameters;
        outputParameters.device = Pa_GetDefaultOutputDevice();
        if (outputParameters.device == paNoDevice) std::cerr << "Audio :: Error :: No default output device.\n";
        outputParameters.channelCount = d.sfinfo->channels;
        outputParameters.sampleFormat = paFloat32;
        outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
        outputParameters.hostApiSpecificStreamInfo = NULL;
        
        PaStream *stream;
        PaError err = Pa_OpenStream(&stream, NULL, &outputParameters, d.sfinfo->samplerate, 0, paClipOff,
                                    [](const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData) -> int
                                    {
                                        AudioData *d = (AudioData*)userData;
                                        float *out = (float*)outputBuffer;
                                        framesPerBuffer *= d->sfinfo->channels;
                                        
                                        sf_count_t readCount = 0;
                                        float data[framesPerBuffer];
                                        if ((readCount = sf_read_float(d->file, data, framesPerBuffer)))
                                            memcpy(out, data, sizeof(float) * framesPerBuffer);
                                        else return paComplete;
                                        return paContinue;
                                    }, &d);
        if (err != paNoError) std::cerr << "Error :: Audio :: " << Pa_GetErrorText( err ) << ".\n";
        
        err = Pa_SetStreamFinishedCallback(stream,
                                           [](void* userData)
                                           {
                                               AudioData *d = (AudioData*)userData;
                                               d->playing = false;
                                               printf("Stream Completed\n");
                                           });
        if (err != paNoError) std::cerr << "Error :: Audio :: " << Pa_GetErrorText( err ) << ".\n";
        err = Pa_StartStream(stream); if (err != paNoError) std::cerr << "Error :: Audio :: " << Pa_GetErrorText( err ) << ".\n";
        
        // Waiting til the Stream ends...
        do { if (!Pa_IsStreamActive(stream)) break; Pa_Sleep(1); } while (1);
        
        // Closing the Stream
        err = Pa_StopStream(stream); if (err != paNoError) std::cerr << "Error :: Audio :: " << Pa_GetErrorText( err ) << ".\n";
        err = Pa_CloseStream(stream); if (err != paNoError) std::cerr << "Error :: Audio :: " << Pa_GetErrorText( err ) << ".\n";
        
        sf_close(d.file); // Closing the sndfile's File
    }
}

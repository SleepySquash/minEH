//
//  Audio.cpp
//  minEH
//
//  Created by Никита Исаенко on 27.01.2021.
//

#include "Audio.hpp"

namespace mh
{
    void Audio::Init()
    {
        PaError err = Pa_Initialize();
        if (err != paNoError) std::cerr << "Error :: Audio :: " << Pa_GetErrorText( err ) << ".\n";
    }
    void Audio::Destroy() { Pa_Terminate(); }
    
    
    Audio::Audio() { }
    Audio::Audio(const std::string& filename) { open(filename); }
    Audio::~Audio()
    {
        d.mux.lock();
        if (stream) Pa_AbortStream(stream);
        if (stream) Pa_CloseStream(stream);
        if (d.file) sf_close(d.file);
        if (d.sfinfo) delete d.sfinfo;
        d.mux.unlock();
    }
    
    void Audio::open(const std::string& filename)
    {
        close(); d.sfinfo = new SF_INFO;
        if (!(d.file = sf_open(filename.c_str(), SFM_READ, d.sfinfo))) { printf("Error :: Audio :: Failed to open file: \"%s\".\n", filename.c_str()); return; }
        duration = (double)d.sfinfo->frames / d.sfinfo->samplerate; isOpen = true;
    }
    void Audio::close()
    {
        d.mux.lock();
        if (stream) Pa_AbortStream(stream);
        if (stream) { Pa_CloseStream(stream); stream = nullptr; }
        if (d.file) { sf_close(d.file); d.file = nullptr; }
        if (d.sfinfo) { delete d.sfinfo; d.sfinfo = nullptr; }
        d.playing = false; d.framesRead = 0; isOpen = false;
        d.mux.unlock();
    }
    
    void Audio::play()
    {
        // Create PortAudio's Device and Stream
        PaStreamParameters outputParameters;
        outputParameters.device = Pa_GetDefaultOutputDevice();
        if (outputParameters.device == paNoDevice) { std::cerr << "Error :: Audio :: No default output device.\n"; return; }
        outputParameters.channelCount = d.sfinfo->channels;
        outputParameters.sampleFormat = paFloat32;
        outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
        outputParameters.hostApiSpecificStreamInfo = NULL;
        
        PaError err = Pa_OpenStream(&stream, NULL, &outputParameters, d.sfinfo->samplerate, 0, paClipOff,
                                    [](const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData) -> int
                                    {
                                        MyAudioData *d = (MyAudioData*)userData;
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
        
        err = Pa_SetStreamFinishedCallback(stream, [](void* userData) { ((MyAudioData*)userData)->playing = false; });
        if (err != paNoError) std::cerr << "Error :: Audio :: " << Pa_GetErrorText( err ) << ".\n";
        err = Pa_StartStream(stream); if (err != paNoError) std::cerr << "Error :: Audio :: " << Pa_GetErrorText( err ) << ".\n"; else d.playing = true;
    }
    
    void Audio::pause() { stop(); }
    void Audio::stop() { PaError err = Pa_StopStream(stream); if (err != paNoError) std::cerr << "Error :: Audio :: " << Pa_GetErrorText( err ) << ".\n"; }
    
    void Audio::setLooped(const bool& doLoop) { d.looped = true; }
    bool Audio::isLooped() { return d.looped; }
    bool Audio::isPlaying() { return d.playing; }
    double Audio::getPosition() { return (double)d.framesRead / d.sfinfo->samplerate; }
    void Audio::setPosition(const double &seconds) { d.framesRead = (seconds * d.sfinfo->samplerate); if (d.file) sf_seek(d.file, d.framesRead, SEEK_SET); }
    double Audio::getDuration() { return duration; }
    
    void Audio::test(const std::string& filename)
    {
        // Open sndfile's Audio File
        MyAudioData d; d.sfinfo = new SF_INFO;
        
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
                                        MyAudioData *d = (MyAudioData*)userData;
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
                                               MyAudioData *d = (MyAudioData*)userData;
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

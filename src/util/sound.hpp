#ifndef SOUND_H
#define SOUND_H

#include <RtAudio.h>
#include <atomic>
#include <iostream>
#include <vector>
#include <cmath>

class Sound {
public:
    Sound();
    ~Sound();

    bool start();        // start capturing
    void stop();         // stop capturing
    float getLevel() const; // thread-safe dB read

private:
    RtAudio adc;
    std::atomic<float> current_dB{0.0f};
    bool running = false;

    static int static_callback(void *outputBuffer, void *inputBuffer,
                               unsigned int nBufferFrames, double streamTime,
                               RtAudioStreamStatus status, void *userData);

    int callback(void *outputBuffer, void *inputBuffer,
                 unsigned int nBufferFrames, double streamTime,
                 RtAudioStreamStatus status);
};

#endif // SOUND_H

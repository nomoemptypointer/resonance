#include <portaudio.h>
#include <iostream>
#include "../include/resonance.h"

// PortAudio callback
static int paCallback(const void* inputBuffer, void* outputBuffer,
                      unsigned long framesPerBuffer,
                      const PaStreamCallbackTimeInfo* timeInfo,
                      PaStreamCallbackFlags statusFlags, void* userData) {
    float* out = (float*)outputBuffer;
    resonance_update(out, framesPerBuffer);
    return paContinue;
}

int main() {
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "PortAudio init error: " << Pa_GetErrorText(err) << "\n";
        return 1;
    }

    resonance_init(48000);

    PaStream* stream;
    err = Pa_OpenDefaultStream(&stream, 0, 1, paFloat32, 48000, 256, paCallback, nullptr);
    if (err != paNoError) {
        std::cerr << "PortAudio open stream error: " << Pa_GetErrorText(err) << "\n";
        return 1;
    }

    err = Pa_StartStream(stream);
    if (err != paNoError) {
        std::cerr << "PortAudio start stream error: " << Pa_GetErrorText(err) << "\n";
        return 1;
    }

    std::cout << "Playing for 5 seconds...\n";
    Pa_Sleep(5000);

    Pa_StopStream(stream);
    Pa_CloseStream(stream);

    resonance_shutdown();
    Pa_Terminate();

    return 0;
}

#include <SDL3/SDL.h>
#include <iostream>
#include <vector>
#include <csignal>
#include "../include/resonance.h"

static bool running = true;
static void handleSignal(int) { running = false; }

static void SDLCALL audioCallback(void* userdata, SDL_AudioStream* stream, int additional_amount, int /*total_amount*/){
    const int channels = 2;
    size_t frames = additional_amount / (sizeof(Resonance::Sample) * channels);

    static std::vector<Resonance::Sample> buffer;
    buffer.resize(frames * channels);

    Resonance::Update(buffer.data(), frames);

    SDL_PutAudioStreamData(stream, buffer.data(), additional_amount);
}

int main() {
    std::signal(SIGINT, handleSignal);
    std::signal(SIGTERM, handleSignal);

    if (!SDL_Init(SDL_INIT_AUDIO)) {
        std::cerr << "SDL Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    const uint32_t sampleRate = 48000;
    const int channels = 2;

    Resonance::Initialize(sampleRate);

    SDL_AudioSpec spec;
    SDL_zero(spec);
    spec.freq = sampleRate;
    spec.format = SDL_AUDIO_F32;
    spec.channels = channels;

    SDL_AudioStream* stream = SDL_OpenAudioDeviceStream(
        SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
        &spec,
        audioCallback,
        nullptr
    );

    SDL_ResumeAudioStreamDevice(stream);

    if (!stream) {
        std::cerr << "Failed to open SDL audio device: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    std::cout << "Playing indefinitely. Press Ctrl+C to stop.\n";

    while (running) {
        SDL_Delay(1);
    }

    std::cout << "\nStopping audio...\n";

    Resonance::Shutdown();
    SDL_CloseAudioDevice(SDL_GetAudioStreamDevice(stream));
    SDL_DestroyAudioStream(stream);
    SDL_Quit();

    return 0;
}

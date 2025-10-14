#include <SDL3/SDL.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <csignal>
#include <fstream>

#include "Sound.h"
#include "../include/resonance.h"

static bool running = true;
static void handleSignal(int) { running = false; }

// Audio callback
static void SDLCALL audioCallback(void* userdata, SDL_AudioStream* stream, int additional_amount, int /*total_amount*/) {
    constexpr int channels = 2;
    size_t frames = additional_amount / (sizeof(Resonance::Sample) * channels);

    static std::vector<Resonance::Sample> buffer;
    buffer.resize(frames * channels);

    // Fill buffer using Resonance engine
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

    constexpr uint32_t sampleRate = 48000;
    constexpr int channels = 2;

    Resonance::Initialize(sampleRate, Resonance::StartupFlags::Default);

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

    if (!stream) {
        std::cerr << "Failed to open SDL audio device: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_ResumeAudioStreamDevice(stream);

    // --- Load raw PCM float32 file ---
    std::ifstream file("pluck.ieee", std::ios::binary | std::ios::ate);
    if (!file) {
        std::cerr << "Failed to open pluck.ieee\n";
        return 1;
    }

    // Get file size
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    // Read file into vector
    std::vector<float> buffer(size / sizeof(float));
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        std::cerr << "Failed to read file data\n";
        return 1;
    }

    Resonance::Sound sound;
    if (!sound.LoadFromMemory(buffer.data(), buffer.size() * sizeof(float), 1, 48000)) {
        std::cerr << "Failed to load sound\n";
        return 1;
    }

    Resonance::RegisterSound(&sound);

    sound.SetVolume(1.0f);
    sound.SetPan(0.0f);

    std::cout << "Press Ctrl+C to stop.\n";

    while (running) {
        SDL_Delay(2500);
        sound.Play();
    }

    std::cout << "\nStopping audio...\n";

    Resonance::Shutdown();
    SDL_CloseAudioDevice(SDL_GetAudioStreamDevice(stream));
    SDL_DestroyAudioStream(stream);
    SDL_Quit();

    return 0;
}

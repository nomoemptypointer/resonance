#include <chrono>
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

    std::ifstream file("pluckStereo.pcm", std::ios::binary | std::ios::ate);
    if (!file) {
        std::cerr << "Failed to open pcm\n";
        return 1;
    }

    // Get file size
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    // Read file as int16_t
    std::vector<int16_t> buffer(size / sizeof(int16_t));
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        std::cerr << "Failed to read file data\n";
        return 1;
    }

    Resonance::Sound sound;
    // Pass raw int16_t data directly
    if (!sound.LoadFromMemory(buffer.data(), buffer.size() * sizeof(int16_t), 2, 48000)) {
        std::cerr << "Failed to load sound\n";
        return 1;
    }

    sound.SetVolume(0.1f);
    sound.SetPan(0.0f);
    Resonance::SetMaxConcurrentSounds(500);

    std::cout << "Press Ctrl+C to stop.\n";

    auto lastTime = std::chrono::high_resolution_clock::now();
    constexpr double targetFrameTimeMs = 1.0; // your SDL_Delay(1) target

    while (running) {
        auto now = std::chrono::high_resolution_clock::now();
        double deltaMs = std::chrono::duration<double, std::milli>(now - lastTime).count();
        lastTime = now;
        sound.Play();

        std::cout << "\rVoice count: " << Resonance::GetCurrentConcurrentSounds() << std::flush;
        if (deltaMs > targetFrameTimeMs * 2) { // arbitrary tolerance
            std::cerr << "\nWarning: main loop running slow! delta = " << deltaMs << " ms\n";
        }

        SDL_Delay(1); // small sleep to prevent CPU hog
    }
    std::cout << "\nStopping audio...\n";

    Resonance::Shutdown();
    SDL_CloseAudioDevice(SDL_GetAudioStreamDevice(stream));
    SDL_DestroyAudioStream(stream);
    SDL_Quit();

    return 0;
}

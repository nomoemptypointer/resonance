#include "../include/resonance.h"

#include <iostream>
#include <vector>
#include <cmath>
#include <cstddef>

static bool g_running = false;
static uint32_t g_sampleRate = 48000;
static float g_phase = 0.0f;

struct MixerTrack {
    bool active = true;
    float volume = 1.0f;
    float* buffer = nullptr;   // audio stream
    size_t bufferSize = 0;     // size of buffer
    size_t readPos = 0;        // current playback position
};

static std::vector<MixerTrack> g_tracks;

extern "C" {

    void resonance_init(uint32_t sampleRate) {
        if (g_running) return;
        g_sampleRate = sampleRate;
        g_running = true;

        // Initialize a single track for testing
        g_tracks.push_back({true, 0.5f});

        std::cout << "[Resonance] Audio engine initialized.\n";
    }

    void resonance_shutdown() {
        if (!g_running) return;
        g_running = false;
        g_tracks.clear();
        g_phase = 0.0f;
        std::cout << "[Resonance] Audio engine shut down.\n";
    }

    void resonance_update(float* buffer, size_t frames) {
        if (!g_running) return;

        for (size_t i = 0; i < frames; i++) {
            float sample = 0.0f;

            // Mix all active tracks
            for (auto& track : g_tracks) {
                if (!track.active) continue;

                // If track has a buffer, mix from it
                if (track.buffer && track.bufferSize > 0) {
                    sample += track.buffer[track.readPos] * track.volume;
                    track.readPos = (track.readPos + 1) % track.bufferSize;
                }

                // Otherwise, do nothing yet (no procedural audio)
            }

            buffer[i] = sample;
        }
    }
}
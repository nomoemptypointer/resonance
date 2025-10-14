#include "../include/Resonance.h"
#include "../include/Sound.h"

#include <iostream>
#include <memory>
#include <mutex>
#include <algorithm>

namespace Resonance {
    static bool g_running = false;
    static uint32_t g_sampleRate = 48000;
    static uint8_t g_channels = 2;
    static float g_volume = 1;

    static std::vector<Sound*> g_activeSounds;
    static std::mutex g_mutex;

    float Clamp(float v, float minVal, float maxVal) {
        if (v < minVal) return minVal;
        if (v > maxVal) return maxVal;
        return v;
    }

    void Initialize(uint32_t sampleRate, StartupFlags startupFlags) {
        if (g_running) return;
        g_sampleRate = sampleRate;
        if (any(startupFlags, StartupFlags::Mono))
            g_channels = 1;
        g_running = true;
    }

    void Shutdown() {
        if (!g_running) return;
        g_running = false;
    }

    void Update(Sample* buffer, size_t frames) {
        if (!g_running)
            return;

        std::lock_guard<std::mutex> lock(g_mutex);

        for (size_t i = 0; i < frames; i++) {
            float left = 0.0f;
            float right = 0.0f;

            for (auto* s : g_activeSounds) {
                if (!s->IsPlaying())
                    continue;

                uint32_t pos = s->m_position;
                if (pos >= s->GetLength()) {
                    s->Stop();
                    continue;
                }

                float sample = s->m_buffer[pos * s->GetChannels()]; // assume mono for now

                // Apply panning
                float pan = s->GetPan(); // -1 = left, 0 = center, 1 = right
                float lGain = (pan <= 0) ? 1.0f : 1.0f - pan;
                float rGain = (pan >= 0) ? 1.0f : 1.0f + pan;

                sample *= s->GetVolume();

                left  += sample * lGain;
                right += sample * rGain;

                s->m_position++;
            }

            // Clamp and apply master volume
            left  = Clamp(left * g_volume, -1.0f, 1.0f);
            right = Clamp(right * g_volume, -1.0f, 1.0f);

            if (g_channels == 2) {
                buffer[i * 2 + 0] = left;
                buffer[i * 2 + 1] = right;
            } else {
                buffer[i] = (left + right) * 0.5f;
            }
        }
    }

    void SetMasterVolume(float volume) {
        if (volume < 0.0f) volume = 0.0f;
        g_volume = volume;
    }

    float GetMasterVolume() {
        return g_volume;
    }

    void RegisterSound(Sound* s) {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_activeSounds.push_back(s);
    }

    void UnregisterSound(Sound* s) {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_activeSounds.erase(
            std::remove(g_activeSounds.begin(), g_activeSounds.end(), s),
            g_activeSounds.end()
        );
    }
}

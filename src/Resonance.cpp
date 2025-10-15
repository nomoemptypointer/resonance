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

    struct SoundInstance {
        Sound* sound;       // Reference to the original sound data
        uint32_t position;  // Current playback position
        float volume;       // Instance-specific volume
        float pan;          // Instance-specific pan
        bool isPlaying;     // Is this instance active?
    };

    static std::vector<SoundInstance> g_activeInstances;
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

            for (auto& instance : g_activeInstances) {
                if (!instance.isPlaying)
                    continue;

                Sound* s = instance.sound;
                uint32_t pos = instance.position;
                uint32_t channels = s->GetChannels();
                uint32_t length = s->GetLength();

                // Stop instance if finished
                if (pos >= length) {
                    instance.isPlaying = false;
                    continue;
                }

                // Fetch samples
                float sampleL = 0.0f;
                float sampleR = 0.0f;

                if (channels == 1) {
                    float sample = s->m_buffer[pos];
                    sampleL = sampleR = sample;
                } else if (channels >= 2) {
                    sampleL = s->m_buffer[pos * channels + 0];
                    sampleR = s->m_buffer[pos * channels + 1];
                }

                // Apply instance volume
                sampleL *= instance.volume;
                sampleR *= instance.volume;

                // Apply instance pan
                float pan = instance.pan;
                float lGain = (pan <= 0) ? 1.0f : 1.0f - pan;
                float rGain = (pan >= 0) ? 1.0f : 1.0f + pan;

                left  += sampleL * lGain;
                right += sampleR * rGain;

                instance.position++;
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

        // Remove finished instances
        g_activeInstances.erase(
            std::remove_if(g_activeInstances.begin(), g_activeInstances.end(),
                           [](const SoundInstance& inst) { return !inst.isPlaying; }),
            g_activeInstances.end()
        );
    }

    void SetMasterVolume(float volume) {
        if (volume < 0.0f) volume = 0.0f;
        g_volume = volume;
    }

    float GetMasterVolume() {
        return g_volume;
    }

    void PlaySound(Sound* s) {
        if (!s || s->m_buffer.empty())
            return;

        std::lock_guard<std::mutex> lock(g_mutex);

        SoundInstance instance{};
        instance.sound = s;
        instance.position = 0;
        instance.volume = s->GetVolume();
        instance.pan = s->GetPan();
        instance.isPlaying = true;

        g_activeInstances.push_back(instance);
    }
}

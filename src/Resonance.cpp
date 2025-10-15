#include "../include/Resonance.h"
#include "../include/Sound.h"

#include <iostream>
#include <memory>
#include <mutex>
#include <algorithm>
#include <immintrin.h>

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
        float pitch;        // Pitch factor
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
        if (!g_running) return;

        std::lock_guard<std::mutex> lock(g_mutex);
        std::fill_n(buffer, frames * g_channels, 0.0f);

        for (auto& instance : g_activeInstances) {
            if (!instance.isPlaying) continue;

            Sound* s = instance.sound;
            uint32_t channels = s->GetChannels();
            uint32_t length = s->GetLength();
            uint32_t pos = instance.position;

            // Precompute gains
            float pan = instance.pan;
            float lGain = instance.volume * ((pan <= 0) ? 1.0f : 1.0f - pan);
            float rGain = instance.volume * ((pan >= 0) ? 1.0f : 1.0f + pan);

            for (size_t i = 0; i < frames && pos < length; i++, pos++) {
                float sampleL, sampleR;

                if (channels == 1) {
                    float sample = s->m_buffer[pos];
                    sampleL = sampleR = sample;
                } else {
                    float* ptr = s->m_buffer.data() + pos * channels;
                    sampleL = ptr[0];
                    sampleR = ptr[1];
                }

                if (g_channels == 2) {
                    buffer[i * 2 + 0] += sampleL * lGain;
                    buffer[i * 2 + 1] += sampleR * rGain;
                } else {
                    buffer[i] += 0.5f * (sampleL * lGain + sampleR * rGain);
                }
            }

            instance.position = pos;
            if (pos >= length)
                instance.isPlaying = false;
        }

        // Apply master volume and clamp
        if (g_channels == 2) {
            for (size_t i = 0; i < frames * 2; i++) {
                buffer[i] = Clamp(buffer[i] * g_volume, -1.0f, 1.0f);
            }
        } else {
            for (size_t i = 0; i < frames; i++) {
                buffer[i] = Clamp(buffer[i] * g_volume, -1.0f, 1.0f);
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

    size_t GetCurrentVoiceCount() {
        return g_activeInstances.size();
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

#include "../include/Resonance.h"

#include <iostream>
#include <memory>

namespace Resonance {
    static bool g_running = false;
    static uint32_t g_sampleRate = 48000;

    void Initialize(uint32_t sampleRate) {
        if (g_running) return;
        g_sampleRate = sampleRate;
        g_running = true;
    }

    void Shutdown() {
        if (!g_running) return;
        g_running = false;
    }

    void Update(Sample* buffer, size_t frames) {
        if (!g_running) return;

        for (size_t i = 0; i < frames; i++) {
            Sample s = 0.0f;

            // for (auto& track : g_channels) { ... }

            buffer[i] = s;
        }
    }
}

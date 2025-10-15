#ifndef RESONANCE_LIBRARY_H
#define RESONANCE_LIBRARY_H

#include <cstdint>
#include <cstddef>

#include "Types.h"
#include "Sound.h"
#include "internal/import.h"

namespace Resonance {
    RESONANCE_API void Initialize(uint32_t sampleRate, StartupFlags startupFlags);
    RESONANCE_API void Shutdown();
    RESONANCE_API void Update(Sample* buffer, size_t frames);
    RESONANCE_API void SetMasterVolume(float volume);
    RESONANCE_API float GetMasterVolume();
    RESONANCE_API size_t GetCurrentConcurrentSounds();
    RESONANCE_API uint32_t GetMaxConcurrentSounds();
    RESONANCE_API void SetMaxConcurrentSounds(uint32_t maxConcurrentSounds);
    RESONANCE_API void PlaySound(Sound* s);
}

#endif

#ifndef RESONANCE_LIBRARY_H
#define RESONANCE_LIBRARY_H

#include <cstdint>
#include <cstddef>

#include "Types.h"

#if defined(_MSC_VER) || defined(__MINGW32__)
    #ifdef RESONANCE_EXPORTS
        #define RESONANCE_API __declspec(dllexport)
    #else
        #define RESONANCE_API __declspec(dllimport)
    #endif
#else
    #define RESONANCE_API __attribute__((visibility("default")))
#endif

namespace Resonance {
    RESONANCE_API void Initialize(uint32_t sampleRate);
    RESONANCE_API void Shutdown();
    RESONANCE_API void Update(Sample* buffer, size_t frames);
}

#endif

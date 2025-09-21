#ifndef RESONANCE_LIBRARY_H
#define RESONANCE_LIBRARY_H

#if defined(_WIN32) || defined(_WIN64)
  #ifdef RESONANCE_EXPORTS
    #define RESONANCE_API __declspec(dllexport)
  #else
    #define RESONANCE_API __declspec(dllimport)
  #endif
#else
  #define RESONANCE_API
#endif

#include <cstdint>
#include <cstddef>

extern "C" {
  RESONANCE_API void resonance_init(uint32_t sampleRate);
  RESONANCE_API void resonance_shutdown();
  RESONANCE_API void resonance_update(float* buffer, size_t frames);
}

#endif // RESONANCE_LIBRARY_H

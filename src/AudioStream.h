#ifndef RESONANCE_AUDIOSTREAM_H
#define RESONANCE_AUDIOSTREAM_H
#include <cstdint>
#include <vector>

#include "../include/Types.h"

struct AudioStream {
    std::vector<Resonance::Sample> Samples; // Interleaved layout : L R L R L R ...
    uint8_t Channels = 2;                   // Number of channels (1 = mono, 2 = stereo), I think we don't need more
    uint32_t SampleRate = 48000;            // Samples per second

    size_t FrameCount() const {
        return Samples.size() / Channels;
    }

    Resonance::Sample& SampleAt(size_t frameIndex, uint8_t channel) {
        return Samples[frameIndex * Channels + channel];
    }

    const Resonance::Sample& SampleAt(size_t frameIndex, uint8_t channel) const {
        return Samples[frameIndex * Channels + channel];
    }
};

#endif
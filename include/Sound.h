#ifndef RESONANCE_SAMPLE_H
#define RESONANCE_SAMPLE_H

#include <string>
#include <cstdint>
#include <vector>

#include "internal/import.h"

namespace Resonance {
    class RESONANCE_API Sound {
    public:
        Sound() = default;
        ~Sound() = default;

        // Non-copyable
        Sound(const Sound&) = delete;
        Sound& operator=(const Sound&) = delete;

        // Movable
        Sound(Sound&& other) noexcept;
        Sound& operator=(Sound&& other) noexcept;

        // Loading
        bool LoadFromMemory(const void* data, size_t size, uint8_t channels, uint32_t frequency);

        bool Play();
        bool Stop();
        bool IsPlaying() const;

        void SetVolume(float volume);
        float GetVolume() const;

        void SetPan(float pan);
        float GetPan() const;

        uint32_t GetLength() const;
        uint32_t GetFrequency() const;
        uint32_t GetChannels() const;

        std::vector<float> m_buffer;
        size_t m_position = 0;

    private:
        uint32_t m_frequency = 0;
        uint32_t m_channels = 0;
        bool m_isPlaying = false;
        float m_volume = 1.0f;
        float m_pan = 0.0f;
        bool m_loop = false;
    };

} // namespace Resonance

#endif // RESONANCE_SAMPLE_H

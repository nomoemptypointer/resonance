#include "Sound.h"
#include <iostream>
#include <cstring>

#include "resonance.h"

namespace Resonance {
    Sound::Sound(Sound&& other) noexcept {
        *this = std::move(other);
    }

    Sound& Sound::operator=(Sound&& other) noexcept {
        if (this != &other) {
            m_buffer = std::move(other.m_buffer);
            m_frequency = other.m_frequency;
            m_channels = other.m_channels;
            m_isPlaying = other.m_isPlaying;
            m_volume = other.m_volume;
            m_pan = other.m_pan;
            m_loop = other.m_loop;
            m_position = other.m_position;

            // Reset other
            other.m_frequency = 0;
            other.m_channels = 0;
            other.m_isPlaying = false;
            other.m_position = 0;
        }
        return *this;
    }

    bool Sound::LoadFromMemory(const void* data, size_t size, uint8_t channels, uint32_t frequency) {
        if (!data || size == 0)
            return false;

        // Assume data is raw int16_t PCM
        size_t sampleCount = size / sizeof(int16_t);
        m_buffer.resize(sampleCount);

        // Copy the data and convert to float internally if m_buffer is float
        for (size_t i = 0; i < sampleCount; ++i) {
            m_buffer[i] = static_cast<float>(static_cast<const int16_t*>(data)[i]) / 32768.0f;
        }

        m_channels = channels;
        m_frequency = frequency;
        return true;
    }

    bool Sound::Play() {
        Resonance::PlaySound(this);
        return true;
    }

    bool Sound::Stop() {
        if (!m_isPlaying)
            return false;

        m_isPlaying = false;
        m_position = 0;
        return true;
    }

    bool Sound::IsPlaying() const {
        return m_isPlaying;
    }

    void Sound::SetVolume(float volume) {
        if (volume < 0.0f) volume = 0.0f;
        m_volume = volume;
    }

    float Sound::GetVolume() const {
        return m_volume;
    }

    void Sound::SetPan(float pan) {
        if (pan < -1.0f) pan = -1.0f;
        if (pan > 1.0f) pan = 1.0f;
        m_pan = pan;
    }

    float Sound::GetPan() const {
        return m_pan;
    }

    uint32_t Sound::GetLength() const {
        if (m_channels == 0)
            return 0;
        return static_cast<uint32_t>(m_buffer.size() / m_channels);
    }

    uint32_t Sound::GetFrequency() const {
        return m_frequency;
    }

    uint32_t Sound::GetChannels() const {
        return m_channels;
    }
}

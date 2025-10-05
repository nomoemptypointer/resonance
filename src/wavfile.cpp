#include "wavfile.h"
#include <iostream>
#include <cstring>

wavfile::wavfile() = default;

wavfile::~wavfile() {
    close();
}

bool wavfile::open(std::istream& stream) {
    close();
    m_stream = &stream;

    if (!parseHeader()) {
        close();
        return false;
    }

    m_open = true;
    return true;
}

bool wavfile::open(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) return false;
    return open(file);
}

void wavfile::close() {
    m_stream = nullptr;
    reset();
}

bool wavfile::parseHeader() {
    if (!m_stream || !*m_stream) return false;

    m_stream->clear();                  // clear any error flags
    m_stream->seekg(0, std::ios::beg);  // rewind to start

    char riff[4];
    m_stream->read(riff, 4);
    if (std::strncmp(riff, "RIFF", 4) != 0) return false;

    uint32_t riffChunkSize = 0;
    m_stream->read(reinterpret_cast<char*>(&riffChunkSize), 4);

    char wave[4];
    m_stream->read(wave, 4);
    if (std::strncmp(wave, "WAVE", 4) != 0) return false;

    bool fmtFound = false;
    bool dataFound = false;
    uint16_t audioFormat = 0;

    while (*m_stream && !m_stream->eof()) {
        char chunkId[4];
        uint32_t chunkSize = 0;

        m_stream->read(chunkId, 4);
        if (m_stream->eof()) break;
        m_stream->read(reinterpret_cast<char*>(&chunkSize), 4);

        if (std::strncmp(chunkId, "fmt ", 4) == 0) {
            fmtFound = true;

            m_stream->read(reinterpret_cast<char*>(&audioFormat), 2);
            m_stream->read(reinterpret_cast<char*>(&m_channels), 2);
            m_stream->read(reinterpret_cast<char*>(&m_sampleRate), 4);

            // skip byteRate + blockAlign (6 bytes)
            m_stream->seekg(6, std::ios::cur);

            m_stream->read(reinterpret_cast<char*>(&m_bitsPerSample), 2);

            // skip remaining bytes in fmt chunk
            if (chunkSize > 16)
                m_stream->seekg(chunkSize - 16, std::ios::cur);
        }
        else if (std::strncmp(chunkId, "data", 4) == 0) {
            dataFound = true;
            m_dataSize = chunkSize;
            m_dataStart = static_cast<size_t>(m_stream->tellg());
            m_stream->seekg(chunkSize, std::ios::cur);
        }
        else {
            // skip unknown chunk
            m_stream->seekg(chunkSize, std::ios::cur);
        }

        if (fmtFound && dataFound) break;
    }

    if (!fmtFound || !dataFound) return false;

    if (m_bitsPerSample != 8 && m_bitsPerSample != 16 &&
        m_bitsPerSample != 24 && m_bitsPerSample != 32)
        return false;

    // Move stream position to start of data chunk for reading
    m_stream->clear();
    m_stream->seekg(m_dataStart, std::ios::beg);

    return true;
}

size_t wavfile::read(float* outBuffer, size_t sampleCount) {
    if (!m_stream || !*m_stream || m_dataSize == 0) return 0;

    size_t bytesPerSample = m_bitsPerSample / 8;
    size_t totalSamples = m_dataSize / bytesPerSample;

    auto pos = m_stream->tellg();
    if (pos == std::streampos(-1)) return 0;

    size_t currentPos = static_cast<size_t>(pos) - m_dataStart;
    size_t samplesLeft = (currentPos < totalSamples * bytesPerSample)
                             ? (totalSamples - currentPos / bytesPerSample)
                             : 0;

    size_t samplesToRead = std::min(sampleCount, samplesLeft);

    for (size_t i = 0; i < samplesToRead; ++i) {
        float sampleFloat = 0.0f;

        switch (m_bitsPerSample) {
            case 8: {  // unsigned 8-bit PCM
                uint8_t sample8 = 0;
                m_stream->read(reinterpret_cast<char*>(&sample8), 1);
                if (!*m_stream) return i;
                sampleFloat = (static_cast<float>(sample8) - 128.0f) / 128.0f;
                break;
            }
            case 16: { // signed 16-bit PCM
                int16_t sample16 = 0;
                m_stream->read(reinterpret_cast<char*>(&sample16), 2);
                if (!*m_stream) return i;
                sampleFloat = static_cast<float>(sample16) / 32768.0f;
                break;
            }
            case 32: { // IEEE float PCM
                float sample32 = 0.0f;
                m_stream->read(reinterpret_cast<char*>(&sample32), 4);
                if (!*m_stream) return i;
                sampleFloat = sample32;
                break;
            }
            default:
                return 0; // unsupported bitsPerSample
        }
        outBuffer[i] = sampleFloat;
    }

    return samplesToRead;
}

bool wavfile::seek(size_t sampleIndex) {
    // TODO: implement sample seeking between dataStart and dataSize
    return false;
}

uint32_t wavfile::sampleRate() const { return m_sampleRate; }
uint16_t wavfile::channels() const { return m_channels; }
uint16_t wavfile::bitsPerSample() const { return m_bitsPerSample; }
size_t wavfile::totalSamples() const { return m_dataSize / (m_bitsPerSample / 8); }
bool wavfile::isOpen() const { return m_open; }

void wavfile::reset() {
    m_sampleRate = 0;
    m_channels = 0;
    m_bitsPerSample = 0;
    m_dataStart = 0;
    m_dataSize = 0;
    m_open = false;
}

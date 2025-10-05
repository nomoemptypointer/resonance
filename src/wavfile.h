#ifndef RESONANCE_WAVFILE_H
#define RESONANCE_WAVFILE_H

#include <string>
#include <cstdint>
#include <vector>
#include <fstream>

class wavfile {
public:
    wavfile();
    ~wavfile();

    bool open(std::istream& stream);
    bool open(const std::string &path);

    void close();

    // Read a number of samples (per channel) into a provided buffer
    // Returns number of samples actually read
    size_t read(float* outBuffer, size_t sampleCount);

    // Seek to a specific sample index (from start of data)
    bool seek(size_t sampleIndex);

    // Getters
    uint32_t sampleRate() const;
    uint16_t channels() const;
    uint16_t bitsPerSample() const;
    size_t totalSamples() const;
    bool isOpen() const;

private:
    bool parseHeader();
    void reset();

private:
    std::istream* m_stream = nullptr;
    std::string m_path;

    // WAV metadata
    uint32_t m_sampleRate = 0;
    uint16_t m_channels = 0;
    uint16_t m_bitsPerSample = 0;
    size_t   m_dataStart = 0;
    size_t   m_dataSize = 0;

    bool m_open = false;
};

#endif
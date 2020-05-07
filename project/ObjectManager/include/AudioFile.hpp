#ifndef AUDIOFILE_HPP_
#define AUDIOFILE_HPP_

#include <iostream>
#include <bit>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <sstream>


class AudioFile
{
    public:
        AudioFile() = default;
        virtual ~AudioFile() = default;

        bool load(std::string filePath);

        std::string getFilePath() const { return filePath; }

        std::uint8_t getChannelsCount() const { return channels; }

        std::int32_t getSampleRate() const { return sampleRate; };

        std::uint8_t getBitsPerSample() const { return bitsPerSample; };

        std::int32_t getSize() const { return size; }

        std::vector<char> data;

    protected:
    private:

        std::string filePath;
        std::uint8_t channels;
        std::int32_t sampleRate;
        std::uint8_t bitsPerSample;
        std::int32_t size;

        std::int32_t convertToInt(char* buffer, std::size_t len);

        bool loadWavFileHeader(std::ifstream& file);

        bool loadWav();
};

#endif /* !AUDIOFILE_HPP_ */

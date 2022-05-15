#ifndef READER_WAV_H
#define READER_WAV_H

#include <iostream>
#include <sstream>
#include <mutex>

#include <thread>
#include <chrono>
#include <iomanip>
#include <vector>
#include <climits>

// for memory mapping
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

#include "../librosa/librosa.h"

// ALSA
#include <alsa/asoundlib.h>
#define PCM_DEVICE "default"

struct WAV {
    std::string chunk_id = "";
    size_t chunk_size = 0;
    std::string chunk_format = "";
    
    std::string sub_chunk_id = "";
    size_t sub_chunk_size = 0;
    std::string sub_chunk_format = "";
    
    size_t channel = 0;
    size_t sample_rate = 0;
    size_t byte_rate = 0;
    size_t block_align = 0;
    size_t bit_per_sample = 0;
    size_t data_chunk_size = 0;
    
    size_t offset_first_sample = 0;
    
    long num_sample() {
        if (data_chunk_size != 0 && block_align != 0) {
            long rtn = data_chunk_size / block_align;
            return rtn;
        }
        else
            return 0;
    }
    
    long num_sample_per_second() {
        if (sample_rate != 0 && channel != 0) {
            long rtn = sample_rate * channel;
            return rtn;
        }
        else
            return 0;
    }
};

class IReaderWAV {
public:
    virtual void onAmplitudeTimeSeries(std::vector<float> signal) = 0;
};

class ReaderWav {
public:
    ReaderWav();
    
    void stopWork();
    bool isStop() const;
    
    void onSample(IReaderWAV* ireader);
    void onRandom(IReaderWAV* ireader);
    void onNote(IReaderWAV* ireader);
    
protected:
    mutable std::mutex mMutex;
    
    bool mShallStop;
    bool mIsStopped;
    
private:
    WAV setWAV(uint8_t* addr, size_t& offset);
    size_t findOffsetDataHeader(uint8_t* data, size_t& offset, const size_t indicator_reduce);
    std::string getHexOnBufferByte(uint8_t* data, size_t& offset, const size_t limit, const size_t length, bool isLittleEndian);
    
    std::string convertToASCII(const std::string hex);
    std::size_t convertToDecimal(const std::string hex);
    
    void onTimelapse(std::chrono::steady_clock::time_point start, std::chrono::steady_clock::time_point end, std::string msg);
};

#endif
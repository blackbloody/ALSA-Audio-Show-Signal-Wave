#include "reader_wav.h"

void ReaderWav::onSample(IReaderWAV* ireader) {
    
    {
        std::lock_guard<std::mutex> lock(mMutex);
        mIsStopped = false;
    }
    std::cout << "Sample" << std::endl << std::endl;
    
    std::string file_name = "/home/venom/Downloads/ClassroomOfTheEliteHeathens.wav";
    const char* fileName = file_name.c_str();
    
    struct stat fileInfo;
    uint8_t* addr;
    
    int fd = open(fileName, O_RDONLY);
    posix_fadvise(fd, 0, 0, 1);
    fstat(fd, &fileInfo);
    addr = static_cast<uint8_t*>(mmap(NULL, fileInfo.st_size, PROT_READ, MAP_SHARED, fd, 0));
    size_t length = fileInfo.st_size;
    size_t offset = 0;
    WAV obj = setWAV(addr, offset);
    
    std::cout << "Data size: " << obj.data_chunk_size << "\n";
    std::cout << "Byte rate: " << obj.byte_rate << "\n";
    std::cout << "Sample rate: " << obj.sample_rate << "\n";
    std::cout << "Sample size: " << obj.num_sample() << "\n";
    
    float vall = 1 / (float)obj.sample_rate;
    std::cout << "Sample in second: ";
    std::cout << std::setprecision(6) << std::fixed << vall << std::endl;
    std::cout << "Num Sample Per Second: " << obj.num_sample_per_second() << "\n";
    
    ///////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////
    
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    std::vector<float> data = librosa::audio::load(file_name, obj.sample_rate, obj.channel > 1);
    ireader->onAmplitudeTimeSeries(data);
    onTimelapse(start, std::chrono::steady_clock::now(), "//////// GET AMPLITUDE OF TIME SERIES //////////// \n");
    
    unsigned int channels = obj.channel;
    unsigned int sampleRate = obj.sample_rate;
    
    unsigned int tmp;
    int pcm;
    snd_pcm_t *handler;
    snd_pcm_hw_params_t *params;
    
    if ((pcm = snd_pcm_open(&handler, PCM_DEVICE, SND_PCM_STREAM_PLAYBACK, 0))< 0) {
        std::cout << "PLAYBACK OPEN ERROR: " << snd_strerror(pcm) << "\n";
        return;
    }
    
    long microS = 100000;
    //long microS = 100000;
    pcm = snd_pcm_set_params(handler, 
        SND_PCM_FORMAT_S16, // format
        SND_PCM_ACCESS_RW_INTERLEAVED, // access
        channels, // channel
        sampleRate, // rate
        0, // 0 = disallow alsa-lib resample stream, 1 = allow resampling 
        microS // latency (0.5 seconds ... format is in microseconds)
        );
    if (pcm < 0) {
        std::cout << "cannot set hardware parameter : " << snd_strerror(pcm) << "\n";
        return;
    }
    
    long sampleSizePlay = (long)(obj.num_sample_per_second() * (0.012 / 1));
    long frames = (long)((sampleSizePlay * (obj.bit_per_sample / 8)) * obj.channel);
    
    std::cout << "Num Sample: " << sampleSizePlay << "\n";
    std::cout << "Byte: " << frames << "\n";
    
    long balanceData = (long)obj.data_chunk_size;
    long balanceSample = (long)obj.num_sample();
    
    bool isBreakLastOffset = false;
    while(1) {
        
        balanceData = balanceData - frames;
        balanceSample = balanceSample - sampleSizePlay;
        
        if (balanceData == 0 || balanceSample == 0)
            break;
        
        if (balanceSample < sampleSizePlay) {
            isBreakLastOffset = true;
            sampleSizePlay = balanceSample * (0.1 / 1);
            frames = (sampleSizePlay * (obj.bit_per_sample / 8)) * obj.channel;
        }
        
        uint8_t* sample = new uint8_t[(long)frames];
        memcpy(sample, addr + offset, (long)frames);

        snd_pcm_uframes_t frames_play;
        
        frames_play = snd_pcm_writei(handler, sample, sampleSizePlay);
        // balanceData + ":" + balanceSample
        onTimelapse(start, std::chrono::steady_clock::now(), std::to_string(balanceData) + " : " + std::to_string(balanceSample));
        if (frames_play < 0)
            frames_play = snd_pcm_recover(handler, frames_play, 0);
        if (frames_play < 0) {
            std::cout << "cannot writei buffer: " << snd_strerror(frames);
            break;
        }
        offset += frames;
        if (offset > obj.data_chunk_size || isBreakLastOffset)
            break;
    }
    
    {
        std::lock_guard<std::mutex> lock(mMutex);
        
        pcm = snd_pcm_drain(handler);
        if (pcm < 0)
            std::cout << "drain failed: " << snd_strerror(pcm) << "\n";
        snd_pcm_close(handler);
        
        if (munmap(addr, fileInfo.st_size) == -1) {
            close(fd);
        }
        close(fd);
        
        mShallStop = false;
        mIsStopped = true;
    }
    
}

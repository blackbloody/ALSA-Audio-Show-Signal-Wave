#include "reader_wav.h"

void ReaderWav::onNote(IReaderWAV* ireader) {
    
    {
        std::lock_guard<std::mutex> lock(mMutex);
        mIsStopped = false;
    }
    
    std::cout << "Note" << "\n";
    
    float sampleRate = 44100;
    double freq = 261.626;
    double a = 0.5;
    double twoPIf = 2 * M_PI * freq;
    
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    std::vector<float> signal;
    signal.reserve(44100);
    double* buff = new double[44100];
    double* time_milis = new double[44100];
    for (int sample = 0; sample < sampleRate; sample++) {
        //std::this_thread::sleep_for(std::chrono::milliseconds(5));
        {
            //std::lock_guard<std::mutex> lock(mMutex);
            
            double time = sample / sampleRate;
            double resY = a * sin(twoPIf * time);
            
            time_milis[sample] = time; 
            buff[sample] = resY;
            signal.push_back(static_cast<float>(resY));
        }
    }
    
    ireader->onAmplitudeTimeSeries(signal);
    std::cout << std::to_string(time_milis[44099]) << std::endl;
    
    char* byteBuff = new char[44100];
    int idx = 0;
    for (int i = 0; i < 44100;) {
        int x = (int)(buff[idx++]*127);
        char charX = (char)x;
        byteBuff[i++] = charX;
    }
    
    int pcm;
    snd_pcm_t *handler;
    
    if ((pcm = snd_pcm_open(&handler, PCM_DEVICE, SND_PCM_STREAM_PLAYBACK, 0))< 0) {
        std::cout << "PLAYBACK OPEN ERROR: " << snd_strerror(pcm) << "\n";
        return;
    }
    
    long microS = 500000;
    //long microS = 100000;
    pcm = snd_pcm_set_params(handler, 
        SND_PCM_FORMAT_S16, // format
        SND_PCM_ACCESS_RW_INTERLEAVED, // access
        1, // channel
        sampleRate, // rate
        1, // 0 = disallow alsa-lib resample stream, 1 = allow resampling 
        microS // latency (0.5 seconds ... format is in microseconds)
        );
    if (pcm < 0) {
        std::cout << "cannot set hardware parameter : " << snd_strerror(pcm) << "\n";
        return;
    }
    
    //
    float ms_in_small_amount = 0.05 / 1;
    float sampleSizePlay = static_cast<float>(44100 * ms_in_small_amount);
    float frames = static_cast<float>(1 * sampleSizePlay);
    //
    
    snd_pcm_uframes_t frames_play;
    frames_play = snd_pcm_writei(handler, byteBuff, static_cast<int>(frames));
    onTimelapse(start, std::chrono::steady_clock::now(), "Play Note Created: " + std::to_string(frames_play));
    if (frames_play < 0)
        frames_play = snd_pcm_recover(handler, frames_play, 0);
    if (frames_play < 0) {
        std::cout << "cannot writei buffer: " << snd_strerror(frames_play);
    }
    
    {
        std::lock_guard<std::mutex> lock(mMutex);
        
        pcm = snd_pcm_drain(handler);
        if (pcm < 0)
            std::cout << "drain failed: " << snd_strerror(pcm) << "\n";
        snd_pcm_close(handler);
        
        mShallStop = false;
        mIsStopped = true;
    }
}
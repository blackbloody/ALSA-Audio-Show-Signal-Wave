#include "reader_wav.h"

void ReaderWav::onRandom(IReaderWAV* ireader) {
    
    {
        std::lock_guard<std::mutex> lock(mMutex);
        mIsStopped = false;
    }
    
    std::cout << "Random" << "\n";
    
    double tone_freq = 400.0;
    tone_freq = 261.626;;
    int samples_per_sec = 44100;
    
    double output_duration_seconds = 1.0;
    
    int output_sample_count = static_cast<int>(output_duration_seconds * samples_per_sec);
    std::cout << output_sample_count << std::endl;
    
    std::vector<float> signal;
    signal.reserve(44100);
    char* byteBuff = new char[output_sample_count];
    for (int sample_i = 0; sample_i < output_sample_count; ++sample_i) {
        
        double time = sample_i / static_cast<double>(samples_per_sec);
        double amp = std::sin(time * 2 * M_PI * tone_freq);
        
        signal.push_back(static_cast<float>(amp));
        // encode amplitude as a 16-bit, signed integral value
        char sample_value = static_cast<char>(amp * std::numeric_limits<short>::max());
        
        byteBuff[sample_i] = sample_value;
    }
    ireader->onAmplitudeTimeSeries(signal);
    
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
        samples_per_sec, // rate
        0, // 0 = disallow alsa-lib resample stream, 1 = allow resampling 
        microS // latency (0.5 seconds ... format is in microseconds)
        );
    if (pcm < 0) {
        std::cout << "cannot set hardware parameter : " << snd_strerror(pcm) << "\n";
        return;
    }
    
    snd_pcm_uframes_t frames_play;
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    frames_play = snd_pcm_writei(handler, byteBuff, 11025);
    onTimelapse(start, std::chrono::steady_clock::now(), "Play Random Created: " + std::to_string(frames_play));
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
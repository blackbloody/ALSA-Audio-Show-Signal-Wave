#ifndef AUDIO_H_
#define AUDIO_H_

#include <sndfile.h>
#include <samplerate.h>

#include <string.h>
#include <memory>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <exception>
#include <numeric>

#include "util.h"


namespace librosa {
    namespace audio {
        
        static const double BW_FASTEST = 0.85;
        static const double BW_BEST = 0.94759372;
        
        static std::vector<float> load(const std::string& path, int sr=22050, bool mono=true, 
            float offset=0.f, float duration=-1.f, const std::string& dtype="float", const std::string& res_type="kaiser_best") {
            SNDFILE * infile = NULL;
            SF_INFO in_sfinfo;
            
            memset(&in_sfinfo, 0, sizeof(in_sfinfo));
            
            if((infile=sf_open(path.c_str(), SFM_READ, &in_sfinfo)) == NULL) {
                std::cout << "Not able to open output file " << path << std::endl;
                sf_close(infile);
                return std::vector<float>();
            }
            if (in_sfinfo.format != (SF_FORMAT_WAV | SF_FORMAT_PCM_16)) {
                std::cout << "The input file is not WAV format " << std::endl;
                return std::vector<float>();
            }
            
            float* data = new float[in_sfinfo.frames * in_sfinfo.channels];
            sf_readf_float(infile, data, in_sfinfo.frames);
            sf_close(infile);
            
            if (in_sfinfo.channels > 1) {
                for (int i = 0; i < in_sfinfo.frames; ++i)
                    data[i] = std::accumulate(data+i*in_sfinfo.channels, data+(i+1)*in_sfinfo.channels, 0.0) / float(in_sfinfo.channels);
            }
            
            float ratio = float(sr) / float(in_sfinfo.samplerate);
            int out_frames = std::ceil(in_sfinfo.frames*ratio);
            std::vector<float> audio(out_frames);
            
            if (std::abs(ratio-1) < 1e-6) {
                std::cout << "Origin" << std::endl;
                std::copy(data, data+in_sfinfo.frames, audio.begin());
            }
            else {
                std::cout << "Resample" << std::endl;
                SRC_DATA res_data;
                res_data.src_ratio = ratio;
                res_data.input_frames = in_sfinfo.frames;
                res_data.output_frames = out_frames;
                res_data.data_in = data;
                res_data.data_out = audio.data();
                
                // channels (last parameter)
                if (src_simple(&res_data, SRC_SINC_BEST_QUALITY, 1)) {
                    std::cout << "src_simple failed " << std::endl;
                    return std::vector<float>();
                }
            }
            
            delete[]data;
            return audio;
        }
    }
}

#endif
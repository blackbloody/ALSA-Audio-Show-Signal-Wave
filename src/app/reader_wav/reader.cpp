#include "reader_wav.h"

ReaderWav::ReaderWav() : mMutex(), mShallStop(false), mIsStopped(true) {
    
}

void ReaderWav::stopWork() {
    std::lock_guard<std::mutex> lock(mMutex);
    mShallStop = true;
}

bool ReaderWav::isStop() const {
    std::lock_guard<std::mutex> lock(mMutex);
    return mIsStopped;
}

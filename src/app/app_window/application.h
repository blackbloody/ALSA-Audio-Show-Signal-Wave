#ifndef APP_WINDOW_H
#define APP_WINDOW_H

#pragma once

#include <iostream>

#include <gtkmm.h>

#include <thread>

#include "main_dialog.h"
#include "reader_wav.h"

class Application : public Gtk::ApplicationWindow, public IReaderWAV {
public:
    Application(BaseObjectType* obj, Glib::RefPtr<Gtk::Builder> const& builder);
    virtual ~Application() = default;
protected:
    Glib::RefPtr<Gtk::Builder> builder;
    
    void onSample();
    void onRandom();
    void onNote();
    
    void onAmplitudeTimeSeries(std::vector<float> signal);
    
    // update UI
    void showDialog();
    
private:
    Gtk::Button* btnSample; 
    Gtk::Button* btnRandom;
    Gtk::Button* btnNote;
    
    std::thread* mainThread;
    Glib::Dispatcher mDispatcherDialog;
    
    std::vector<float> time_series_main;
    std::vector<float> amplitude_main;
    
    // CLASS HELPER
    ReaderWav mReaderWav;
};

#endif

#include "application.h"

Application::Application(BaseObjectType* obj, Glib::RefPtr<Gtk::Builder> const& builder) : 
Gtk::ApplicationWindow(obj), builder{builder}, mReaderWav() {
    
    set_position(Gtk::WIN_POS_CENTER);
    
    builder->get_widget("btn_sample", btnSample);
    builder->get_widget("btn_random", btnRandom);
    builder->get_widget("btn_note", btnNote);
    
    btnSample->signal_clicked().connect(sigc::mem_fun(*this, &Application::onSample));
    btnRandom->signal_clicked().connect(sigc::mem_fun(*this, &Application::onRandom));
    btnNote->signal_clicked().connect(sigc::mem_fun(*this, &Application::onNote));
    
    mDispatcherDialog.connect(sigc::mem_fun(*this, 
        &Application::showDialog));
}

void Application::onSample() {
    mainThread = new std::thread([this] {
        mReaderWav.onSample(this);
    });
}

void Application::onRandom() {
    mainThread = new std::thread([this] {
        mReaderWav.onRandom(this);
    });
}

void Application::onNote() {
    
    
    ////////////////
    /*
    auto refBuilder = Gtk::Builder::create();
    refBuilder->add_from_file("../src/resources/main_dialog.glade");
    MainDialog* dialog = nullptr;
    refBuilder->get_widget_derived("MainDialog", dialog);
    dialog->set_transient_for(*this);
    
    std::vector<float> ll = {-0.5f, 0.9f, 1.0f, 1.5f, 2.f};
    dialog->showSignal(ll);
    
    dialog->show();
    */
    
    /*
     * // Will wait for result
    int res = dialog->run();
    if (res == Gtk::RESPONSE_DELETE_EVENT) {
        dialog->hide();
    }
    */
    /////////////////
    
    mainThread = new std::thread([this] {
        mReaderWav.onNote(this);
    });
}

void Application::onAmplitudeTimeSeries(std::vector<float> signal) {
    ///*
    std::cout << "Signal Size: " << signal.size() << "\n";
    int min = 0;
    int max = signal.size() - 1;
    std::vector<float> time_series;
    time_series.reserve(signal.size());
    for(int i = 0; i < signal.size(); i++) {
        float val = (2*(((float)i - min)/(max - min)))-1;
        time_series.push_back(val);
    }
    //*/
    time_series_main = time_series;
    amplitude_main = signal;
    mDispatcherDialog.emit();
}

void Application::showDialog() {
    auto refBuilder = Gtk::Builder::create();
    refBuilder->add_from_file("../src/resources/main_dialog.glade");
    MainDialog* dialog = nullptr;
    refBuilder->get_widget_derived("MainDialog", dialog);
    dialog->set_transient_for(*this);
    
    std::vector<float> ll = {-0.5f, 0.9f, 1.0f, 1.5f, 2.f};
    dialog->showSignal(time_series_main, amplitude_main);
    
    dialog->show();
}
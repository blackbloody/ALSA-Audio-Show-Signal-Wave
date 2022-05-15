#include "application.h"

int main(int argc, char *argv[]) {
    
    auto app = Gtk::Application::create(argc, argv, "org.raid.factory");
    Glib::RefPtr<Gtk::Builder> refBuilder = Gtk::Builder::create();
    refBuilder->add_from_file("../src/resources/main_application.glade");
    
    Application* application = nullptr;
    refBuilder->get_widget_derived("MainApplication", application);

    auto r = app->run(*application);

    delete application;
    return r;
}

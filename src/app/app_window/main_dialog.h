#ifndef APP_DIALOG_H
#define APP_DIALOG_H

#include <gtkmm.h>
#include "sfml_widget.h"
#include "openGL_draw.h"

class MainDialog : public Gtk::Dialog {
public:
    MainDialog(BaseObjectType* obj, Glib::RefPtr<Gtk::Builder> const& builder);
    virtual ~MainDialog() = default;
    
    void showSignal(std::vector<float> time_series, std::vector<float> amplitude);
protected:
    Glib::RefPtr<Gtk::Builder> builder;
    
    Gtk::Box* box;
    SFMLWidget* renderWindow;
};

#endif
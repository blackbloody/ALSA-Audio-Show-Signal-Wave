#include "main_dialog.h"

MainDialog::MainDialog(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& builder) : Gtk::Dialog(obj), builder{builder} {
    
    builder->get_widget("container_box", box);
}

void MainDialog::showSignal(std::vector<float> time_series, std::vector<float> amplitude) {
    
    MovingCircle* mc;
    renderWindow = new SFMLWidget(sf::VideoMode(100, 80));
    mc = new MovingCircle(*renderWindow, time_series, amplitude);
    
    box->pack_start(*renderWindow);
    show_all_children();
    
}
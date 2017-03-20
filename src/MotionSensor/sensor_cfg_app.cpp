#include <gtkmm.h>
#include <iostream>
#include "sensor_cfg.h"

int main(int argc, char** argv)
{
    Gtk::Main kit(argc, argv);
    Gui gui;
    gui.get_main_window()->show_all_children();//benötigt für canvas
    kit.run(*gui.get_main_window());

    return 0;
}


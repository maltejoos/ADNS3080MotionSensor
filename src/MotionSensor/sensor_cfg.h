#include <string>
#include <ctime>
#include <sys/time.h>
#include <gtkmm.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <goocanvasmm.h>
#include "MotionSensor.h"

#define depth 		8
#define x_size_scale	100
#define y_size_scale	100

#define canvas_size_x 100
#define canvas_size_y 100

#define motionhead_size 6

using namespace std;

class Gui //: public Gtk::Window
{
    private:

        Glib::RefPtr<Gtk::Builder> gtkbuilder;

        Gtk::Window* mainwindow;
        Gtk::ToggleButton* framebutton;
        Gtk::Label* framelabel;
        Gtk::Image* gtkimage;
        Gtk::Button* readregbutton;
        Gtk::Entry* regentry;
        Gtk::Label* readreglabel;
        Gtk::Button* srombutton;
        Gtk::Button* kalibrationwindowbutton;
        Gtk::Window* kalibrationwindow;
        Gtk::Entry* kalibrationsetentry;
        Gtk::Button* kalibrationsetbutton;
        Gtk::ToggleButton* kalibrationbutton;
        Gtk::ToggleButton* motionbutton;
        Gtk::Box* motioncanvasbox;
        Gtk::VScale* motionscale;
        Gtk::ToggleButton* squalbutton;
        Gtk::Label* squallabel;
        Gtk::Button* resetbutton;
        Gtk::Button* setidbutton;
        Gtk::Statusbar* statusbar;
        Gtk::ToggleButton* tb_connect_dev;
        Gtk::ToggleButton* tb_connect_id;
        Gtk::Entry* deviceentry;
        Gtk::Entry* identry;
        Gtk::Entry* setidentry;
        Gtk::Box* comboboxtextbox;
        Gtk::ComboBoxText baud_comboboxtext;

        Gtk::Notebook* notebook1;
        Gtk::Table* table_exp;
        Gtk::HBox* hbox_main;

        MotionSensor mousehack;


        //alternative geschwindigkeitsmessung
        timeval tv1_alt, tv2_alt;
        double distance;

        //Trajektorie
        int x_offset;
        int y_offset;
        double arrow_scale_factor;
        double canvas_scale_factor;
        Goocanvas::Canvas motion_canvas;
        Glib::RefPtr<Goocanvas::Item> canvas_root;
        Glib::RefPtr<Goocanvas::Polyline> line;
        Goocanvas::Points::Points points;
        Glib::RefPtr<Goocanvas::Rect> motionhead;
        stack<Glib::RefPtr<Goocanvas::Polyline> > linestack;

        //Geschwindigkeit messen
        timeval tv1, tv2;
        double v_abs;

        //Frame
        guchar* rgbimage;
        Glib::RefPtr<Gdk::Pixbuf> gdkpixbuf;

        //Expert Mode
        list<string> fp_list;
        list<string> fpmax_list;
        list<string> fpmin_list;
        list<string> shmax_list;
        list<string> sq_list;
        Gtk::CheckButton* cb_exp_fp, *cb_exp_fpmax, *cb_exp_fpmin, *cb_exp_shmax, *cb_exp_sh;
        Gtk::Entry* entry_exp_fp, *entry_exp_fpmax, *entry_exp_fpmin, *entry_exp_shmax, *entry_exp_sh;
        Gtk::Label* label_exp_fp, *label_exp_fpmax, *label_exp_fpmin, *label_exp_shmax, *label_exp_sq;
        Gtk::ToggleButton *tb_exp_fp, *tb_exp_fpmax, *tb_exp_fpmin, *tb_exp_shmax, *tb_exp_sq, *tb_exp_sh, *tb_exp_all;
        Gtk::Button *button_exp_fp, *button_exp_fpmin, *button_exp_fpmax, *button_exp_shmax, *button_exp_sh;


    public:

        Gui(void);
        Gtk::Window* get_main_window(void);
        void on_readregbutton_clicked(void);
        void on_srombutton_clicked(void);
        void on_motionbutton_toggled(void);
        void on_framebutton_toggled(void);
        bool reload_data(void);
        void on_kalibrationwindowbutton_clicked(void);
        void on_kalibrationsetbutton_clicked(void);
        void on_resetbutton_clicked();
        void on_cb_exp_toggled();
        void button_logic();

        void on_button_exp_fp_clicked();
        void on_button_exp_fpmax_clicked();
        void on_button_exp_fpmin_clicked();
        void on_button_exp_shmax_clicked();
        void on_button_exp_sh_clicked();
        void on_tb_exp_all_toggled();
        void on_button_exp_setid_clicked();

        void on_tb_connect_clicked();
        void on_tb_connect_id_clicked();

        void on_notebook1_switch_page();

        void write_status(string);
};

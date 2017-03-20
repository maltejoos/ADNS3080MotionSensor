#include "sensor_cfg.h"

Gui::Gui()
{
    try{
        gtkbuilder = Gtk::Builder::create_from_file("sensor_cfg.glade");
    }
    catch( Glib::Exception& excp	 )
    {
            cout << excp.what() << endl << endl;
    }

        gtkbuilder->get_widget("mainwindow", mainwindow);

        gtkbuilder->get_widget("notebook1", notebook1);
        //notebook1->signal_switch_page().connect(sigc::mem_fun(this, &Gui::on_notebook1_switch_page));
        gtkbuilder->get_widget("hbox_main", hbox_main);
        gtkbuilder->get_widget("table_exp", table_exp);

        gtkbuilder->get_widget("readregbutton", readregbutton);
        gtkbuilder->get_widget("tb_connect_dev", tb_connect_dev);
        gtkbuilder->get_widget("tb_connect_id", tb_connect_id);
        gtkbuilder->get_widget("deviceentry", deviceentry);
        gtkbuilder->get_widget("identry", identry);
        gtkbuilder->get_widget("regentry", regentry);
        gtkbuilder->get_widget("readreglabel", readreglabel);
        readregbutton->signal_clicked().connect(sigc::mem_fun(this, &Gui::on_readregbutton_clicked));
        tb_connect_dev->signal_clicked().connect(sigc::mem_fun(this, &Gui::on_tb_connect_clicked));
        tb_connect_id->signal_clicked().connect(sigc::mem_fun(this, &Gui::on_tb_connect_id_clicked));


        //gtkbuilder->get_widget("comboboxtextbox", comboboxtextbox);
        //comboboxtextbox->pack_start(baud_comboboxtext, Gtk::PACK_EXPAND_WIDGET, 0);
        //baud_comboboxtext.append_text("57600");
        //baud_comboboxtext.append_text("115200");
        //baud_comboboxtext.set_active_text("57600");

        gtkbuilder->get_widget("srombutton", srombutton);
        srombutton->signal_clicked().connect(sigc::mem_fun(this, &Gui::on_srombutton_clicked));

        gtkbuilder->get_widget("motionbutton", motionbutton);
        motionbutton->signal_clicked().connect(sigc::mem_fun(this, &Gui::on_motionbutton_toggled), Glib::PRIORITY_LOW);

        gtkbuilder->get_widget("squalbutton", squalbutton);
        gtkbuilder->get_widget("squallabel", squallabel);

        gtkbuilder->get_widget("motioncanvasbox", motioncanvasbox);
        motioncanvasbox->pack_start(motion_canvas, Gtk::PACK_EXPAND_WIDGET, 0);

        gtkbuilder->get_widget("motionscale", motionscale);
        motionscale->set_range(1,5);
        motionscale->set_increments(0.1,0.1);

        gtkbuilder->get_widget("resetbutton", resetbutton);
        resetbutton->signal_clicked().connect(sigc::mem_fun(this, &Gui::on_resetbutton_clicked), Glib::PRIORITY_LOW);

        gtkbuilder->get_widget("statusbar", statusbar);


        //Expert Mode
        gtkbuilder->get_widget("cb_exp_fp", cb_exp_fp);
        gtkbuilder->get_widget("cb_exp_fpmax", cb_exp_fpmax);
        gtkbuilder->get_widget("cb_exp_fpmin", cb_exp_fpmin);
        gtkbuilder->get_widget("cb_exp_shmax", cb_exp_shmax);
        gtkbuilder->get_widget("cb_exp_sh", cb_exp_sh);
        cb_exp_fp->signal_clicked().connect(sigc::mem_fun(this, &Gui::on_cb_exp_toggled));
        cb_exp_fpmax->signal_clicked().connect(sigc::mem_fun(this, &Gui::on_cb_exp_toggled));
        cb_exp_fpmin->signal_clicked().connect(sigc::mem_fun(this, &Gui::on_cb_exp_toggled));
        cb_exp_shmax->signal_clicked().connect(sigc::mem_fun(this, &Gui::on_cb_exp_toggled));
        cb_exp_sh->signal_clicked().connect(sigc::mem_fun(this, &Gui::on_cb_exp_toggled));


        gtkbuilder->get_widget("button_exp_fp", button_exp_fp);
        gtkbuilder->get_widget("button_exp_fpmax", button_exp_fpmax);
        gtkbuilder->get_widget("button_exp_fpmin", button_exp_fpmin);
        gtkbuilder->get_widget("button_exp_shmax", button_exp_shmax);
        gtkbuilder->get_widget("button_exp_sh", button_exp_sh);
        button_exp_fp->signal_clicked().connect(sigc::mem_fun(this, &Gui::on_button_exp_fp_clicked));
        button_exp_fpmax->signal_clicked().connect(sigc::mem_fun(this, &Gui::on_button_exp_fpmax_clicked));
        button_exp_fpmin->signal_clicked().connect(sigc::mem_fun(this, &Gui::on_button_exp_fpmin_clicked));
        button_exp_shmax->signal_clicked().connect(sigc::mem_fun(this, &Gui::on_button_exp_shmax_clicked));
        button_exp_sh->signal_clicked().connect(sigc::mem_fun(this, &Gui::on_button_exp_sh_clicked));

        gtkbuilder->get_widget("entry_exp_fp", entry_exp_fp);
        gtkbuilder->get_widget("entry_exp_fpmax", entry_exp_fpmax);
        gtkbuilder->get_widget("entry_exp_fpmin", entry_exp_fpmin);
        gtkbuilder->get_widget("entry_exp_shmax", entry_exp_shmax);
        gtkbuilder->get_widget("entry_exp_sh", entry_exp_sh);

        gtkbuilder->get_widget("label_exp_fp", label_exp_fp);
        gtkbuilder->get_widget("label_exp_fpmax", label_exp_fpmax);
        gtkbuilder->get_widget("label_exp_fpmin", label_exp_fpmin);
        gtkbuilder->get_widget("label_exp_shmax", label_exp_shmax);
        //gtkbuilder->get_widget("label_exp_sh", label_exp_sh);
        gtkbuilder->get_widget("label_exp_sq", label_exp_sq);

        gtkbuilder->get_widget("tb_exp_fp", tb_exp_fp);
        gtkbuilder->get_widget("tb_exp_fpmax", tb_exp_fpmax);
        gtkbuilder->get_widget("tb_exp_fpmin", tb_exp_fpmin);
        gtkbuilder->get_widget("tb_exp_shmax", tb_exp_shmax);
        gtkbuilder->get_widget("tb_exp_sh", tb_exp_sh);
        gtkbuilder->get_widget("tb_exp_sq", tb_exp_sq);
        gtkbuilder->get_widget("tb_exp_all", tb_exp_all);
        tb_exp_all->signal_clicked().connect(sigc::mem_fun(this, &Gui::on_tb_exp_all_toggled));

        gtkbuilder->get_widget("setidentry", setidentry);
        gtkbuilder->get_widget("setidbutton", setidbutton);
        setidbutton->signal_clicked().connect(sigc::mem_fun(this, &Gui::on_button_exp_setid_clicked));

        //Kalibration
        gtkbuilder->get_widget("kalibrationsetentry", kalibrationsetentry);

        gtkbuilder->get_widget("kalibrationwindowbutton", kalibrationwindowbutton);
        kalibrationwindowbutton->signal_clicked().connect(sigc::mem_fun(this, &Gui::on_kalibrationwindowbutton_clicked));

        gtkbuilder->get_widget("kalibrationwindow", kalibrationwindow);

        gtkbuilder->get_widget("kalibrationsetbutton", kalibrationsetbutton);
        kalibrationsetbutton->signal_clicked().connect(sigc::mem_fun(this, &Gui::on_kalibrationsetbutton_clicked));



        //Trajektorie
        x_offset=50;
        y_offset=50;

        arrow_scale_factor=0.05;
        //canvas_scale_factor=1;

        //motion_canvas.set_size_request(50, 50);
        //motion_canvas.set_bounds(0,0,20,20); <-- begrenzt "Malfläche"
        canvas_root = motion_canvas.get_root_item();

        points = Goocanvas::Points::Points(2);
        points.set_coordinate(0, x_offset, y_offset);
        points.set_coordinate(1, x_offset, y_offset);

        line = Goocanvas::Polyline::create(false, points);
        motionhead = Goocanvas::Rect::create(0, 0, motionhead_size, motionhead_size);
        motionhead->property_stroke_color() = "red";
        motionhead->property_fill_color() = "red";
        /*
        line->property_stroke_color() = "blue";
        line->property_line_width() = 1.0;
        line->property_start_arrow() = false;
        line->property_end_arrow() = true;
        line->property_arrow_tip_length() = 5.0;
        line->property_arrow_length() = 6.0;
        line->property_arrow_width() = 6.0;
        */


        linestack.push(line);
        canvas_root->add_child(linestack.top());
        //canvas_root->add_child(motionhead);


        //Frame
        gtkbuilder->get_widget("framebutton", framebutton);
        framebutton->signal_clicked().connect(sigc::mem_fun(this, &Gui::on_framebutton_toggled));

        rgbimage = mousehack.get_frame();

        gdkpixbuf = gdkpixbuf = Gdk::Pixbuf::create_from_data(rgbimage, Gdk::COLORSPACE_RGB, false, depth, mousehack.get_xFramesize(), mousehack.get_yFramesize(), mousehack.get_xFramesize()*3);
        gdkpixbuf = gdkpixbuf->scale_simple(x_size_scale, y_size_scale, Gdk::INTERP_BILINEAR);

        gtkbuilder->get_widget("image", gtkimage);
        gtkimage->set(gdkpixbuf);


        // Besser: I/O callbacks
        sigc::connection con = Glib::signal_idle().connect(sigc::mem_fun(this, &Gui::reload_data), Glib::PRIORITY_LOW);

        button_logic();
}


Gtk::Window* Gui::get_main_window()
{
        return mainwindow;
}


void Gui::on_readregbutton_clicked()
{
    string value;
    int ret;
    value=regentry->get_text();
    guchar reg = strtol(value.c_str(), NULL, 16);

    if(value.length()!=2 || reg<0 || reg>100)
    {

        write_status("Error: Invalid Register");
    }

    else
    {
        stringstream teststream;
        ret = mousehack.read_register(reg);
        teststream << ret;
        readreglabel->set_text(teststream.str());
        cout << ret << endl;
    }

}

void Gui::on_srombutton_clicked()
{
    write_status("Flashing Firmware ...");

    if(mousehack.flash_firmware())
    {
        write_status("Flashing Firmware ... OK");
    }

    else
    {
        write_status("Error: Flashing Firmware failed!");
    }

    //Firmware Status beachten
    if(mousehack.firmware_flashed())
    {
            squalbutton->set_sensitive(true);
            tb_exp_sq->set_sensitive(true);
            motionbutton->set_sensitive(true);
            kalibrationwindowbutton->set_sensitive(true);
    }
}

void Gui::on_framebutton_toggled()
{
    motionbutton->set_sensitive(false);
    kalibrationwindowbutton->set_sensitive(false);

    if(framebutton->get_active())
    {
        framebutton->set_active(true);

        button_exp_fp->set_sensitive(false);
        button_exp_fpmax->set_sensitive(false);
        button_exp_fpmin->set_sensitive(false);
        button_exp_sh->set_sensitive(false);
        button_exp_shmax->set_sensitive(false);
    }

    else{
        mousehack.reset(); //benötigter hardware reset nach frame capture burst

        framebutton->set_active(false);

        button_exp_fp->set_sensitive(true);
        button_exp_fpmax->set_sensitive(true);
        button_exp_fpmin->set_sensitive(true);
        button_exp_sh->set_sensitive(true);
        button_exp_shmax->set_sensitive(true);

        cb_exp_fp->set_active(false);
        cb_exp_fpmax->set_active(false);
        cb_exp_fpmin->set_active(false);
        cb_exp_sh->set_active(false);
        cb_exp_shmax->set_active(false);
    }
}


void Gui::on_motionbutton_toggled()
{
    if(!motionbutton->get_active())
    {
        //  testbetrieb mit leiterwagen und alternativer geschwindigkeitsmessung
        mousehack.stop_integrateMotion();

        gettimeofday(&tv2_alt, NULL);

        unsigned long long t1 = (tv1_alt.tv_sec-1000000000)*1000000 + tv1_alt.tv_usec;
        unsigned long long t2 = (tv2_alt.tv_sec-1000000000)*1000000 + tv2_alt.tv_usec;

        cout << "v average: " << mousehack.get_vAverage() << "m/sec" << endl  << "Standardabweichung: " << mousehack.get_vAverageStandardDeviation() << "m/s" << endl;

        mousehack.setStatisticsStatus(false);
    }

    else if(motionbutton->get_active())
    {
        //  testbetrieb mit leiterwagen und alternativer geschwindigkeitsmessung

        mousehack.setStatisticsStatus(true);

        gettimeofday(&tv1_alt, NULL);

        distance=3;

        mousehack.resetTranslation();

        mousehack.init_integrateMotion();

        points.set_coordinate(0, x_offset, y_offset);
        canvas_root->set_simple_transform(0,0,1,0);
        canvas_root->add_child(motionhead);
        while(!linestack.empty())
        {
            linestack.top()->remove();
            linestack.pop();
        }
    }
}

bool Gui::reload_data()
{
    guchar temp;

    if(framebutton->get_active())
    {
        rgbimage = mousehack.frameCaptureBurst();

        gdkpixbuf = Gdk::Pixbuf::create_from_data(rgbimage, Gdk::COLORSPACE_RGB, false, depth, mousehack.get_xFramesize(), mousehack.get_yFramesize(), mousehack.get_xFramesize()*3);
        gdkpixbuf = gdkpixbuf->scale_simple(x_size_scale, y_size_scale, Gdk::INTERP_NEAREST);
        gtkimage->set(gdkpixbuf);
    }

    if(motionbutton->get_active())
    {
        mousehack.read_integrateMotion();
        //double vx = mousehack.get_vx();
        //double vy = mousehack.get_vy();

        cout << mousehack.get_dx() << "\t" << mousehack.get_dy() << "\t" << endl;

        //trajektorie
        points.set_coordinate(1, x_offset+mousehack.get_xabs()*arrow_scale_factor, y_offset+mousehack.get_yabs()*arrow_scale_factor);
        line = Goocanvas::Polyline::create(0, points);
        linestack.push(line);

        canvas_root->add_child(linestack.top());

        points.set_coordinate(0, x_offset+mousehack.get_xabs()*arrow_scale_factor, y_offset+mousehack.get_yabs()*arrow_scale_factor);

        Gtk::Allocation widget_allocation = motion_canvas.get_allocation();
        double canvas_width = widget_allocation.get_width();
        double canvas_height = widget_allocation.get_height();

        double st_x, st_y, st_scale, st_rotate;
        canvas_root->get_simple_transform(st_x, st_y, st_scale, st_rotate);

        if(y_offset+mousehack.get_yabs()*arrow_scale_factor <= -st_y)
        {
                st_y = -(y_offset+mousehack.get_yabs()*arrow_scale_factor);
        }

        else if(y_offset+mousehack.get_yabs()*arrow_scale_factor > -st_y+canvas_height)
        {
                st_y = -(y_offset+mousehack.get_yabs()*arrow_scale_factor-canvas_height);
        }

        if(x_offset+mousehack.get_xabs()*arrow_scale_factor < -st_x)
        {
                st_x = -(x_offset+mousehack.get_xabs()*arrow_scale_factor);
        }

        else if(x_offset+mousehack.get_xabs()*arrow_scale_factor > -st_x+canvas_width)
        {
                st_x = -(x_offset+mousehack.get_xabs()*arrow_scale_factor-canvas_width);
        }

        canvas_root->set_simple_transform(st_x, st_y, 1./motionscale->get_value(), 0);

        double head_x, head_y;
        points.get_coordinate(0, head_x, head_y);
        motionhead->set_simple_transform(head_x-motionhead_size/2., head_y-motionhead_size/2., 1, 0);
    }

    if(squalbutton->get_active())
    {
        stringstream temp;
        temp << mousehack.get_squal();
        squallabel->set_text(temp.str());
        usleep(50000);
    }

    if(tb_exp_fp->get_active())
    {
        cout << mousehack.get_framePeriod() << "\t";
    }

    if(tb_exp_fpmax->get_active())
    {
        cout << mousehack.get_framePeriodMaxBound() << "\t";
    }

    if(tb_exp_fpmin->get_active())
    {
        cout << mousehack.get_framePeriodMinBound() << "\t";
    }

    if(tb_exp_shmax->get_active())
    {
        cout << mousehack.get_shutterMaxBound() << "\t";
    }

    if(tb_exp_sh->get_active())
    {
        cout << mousehack.get_shutter() << "\t";
    }


    if(tb_exp_sq->get_active())
    {
        cout << mousehack.get_squal() << "\t";
    }

    if(tb_exp_fp->get_active() || tb_exp_fpmax->get_active() || tb_exp_fpmin->get_active() || tb_exp_shmax->get_active() || tb_exp_sh->get_active() || tb_exp_sq->get_active())
    {
            cout << endl;
    }

    usleep(1); //um 100% cpu last im leerlauf zu vermeiden

    return true;
}

void Gui::on_kalibrationwindowbutton_clicked()
{
        kalibrationwindow->show();
}

void Gui::on_kalibrationsetbutton_clicked()
{
        string value = kalibrationsetentry->get_text();

        //ggf. "." durch "," austauschen
        if(value.find(".") != string::npos){
            value.replace(value.find("."), 1, ",");
        }

        mousehack.calibrate_using_image_region(strtod(value.c_str(), NULL));
}

void Gui::on_cb_exp_toggled()
{
        if(cb_exp_fp->get_active()){
            entry_exp_fp->set_sensitive(true);
            //entry_exp_fpmax->set_sensitive(false);
            //cb_exp_fpmax->set_active(false);
            //cb_exp_fpmax->set_sensitive(false);
        }
        else{
            entry_exp_fp->set_sensitive(false);
            //entry_exp_fpmax->set_sensitive(true);
            //cb_exp_fpmax->set_sensitive(true);
        }

        if(cb_exp_fpmax->get_active()){
            entry_exp_fpmax->set_sensitive(true);
        }
        else{
            entry_exp_fpmax->set_sensitive(false);
        }

        if(cb_exp_fpmin->get_active()){
            entry_exp_fpmin->set_sensitive(true);
        }
        else{
            entry_exp_fpmin->set_sensitive(false);
        }

        if(cb_exp_shmax->get_active()){
            entry_exp_shmax->set_sensitive(true);
        }
        else{
            entry_exp_shmax->set_sensitive(false);
        }

        if(cb_exp_sh->get_active()){
            entry_exp_sh->set_sensitive(true);
        }
        else{
            entry_exp_sh->set_sensitive(false);
        }
}

void Gui::button_logic()
{
        if(mousehack.isConnected())
        {
                stringstream temp;
                temp << (int)mousehack.getID();
                setidentry->set_text(temp.str());

                //notebook1->set_sensitive(true);
                hbox_main->set_sensitive(true);
                table_exp->set_sensitive(true);

                if(mousehack.firmware_flashed())
                {
                    motionbutton->set_sensitive(true);
                    kalibrationwindowbutton->set_sensitive(true);
                }

                else
                {
                    motionbutton->set_sensitive(false);
                    kalibrationwindowbutton->set_sensitive(false);
                }


                if(mousehack.automaticFramePeriodActive())
                {
                    cb_exp_fp->set_active(false);
                    entry_exp_fp->set_sensitive(false);
                }

                else
                {
                    entry_exp_fp->set_sensitive(true);
                    stringstream temp;
                    temp << mousehack.get_framePeriod();
                    entry_exp_fp->set_text(temp.str());
                    cb_exp_fp->set_active(true);
                    cb_exp_fp->set_sensitive(true);
                }

                if(mousehack.automaticShutterActive())
                {
                    cb_exp_sh->set_sensitive(true);
                    cb_exp_sh->set_active(false);
                    entry_exp_sh->set_sensitive(false);
                }

                else
                {
                    cb_exp_sh->set_sensitive(true);
                    cb_exp_sh->set_active(true);
                    entry_exp_sh->set_sensitive(true);

                    stringstream temp;
                    temp << mousehack.get_shutter();
                    entry_exp_sh->set_text(temp.str());
                }

                entry_exp_fpmax->set_sensitive(false);
                entry_exp_fpmin->set_sensitive(false);
                entry_exp_shmax->set_sensitive(false);

                cb_exp_fpmax->set_active(false);
                cb_exp_fpmin->set_active(false);
                cb_exp_shmax->set_active(false);
        }

        else
        {
                //notebook1->set_sensitive(false);
                hbox_main->set_sensitive(false);
                table_exp->set_sensitive(false);
        }
}

void Gui::on_button_exp_fp_clicked()
{
        if(cb_exp_fp->get_active())
        {
            string value = entry_exp_fp->get_text();
            int frameperiod = atoi(value.c_str());

            if(frameperiod < 0x0E7E || frameperiod > 0xFFFF){
                write_status("Error: fp zu groß oder klein");
            }
            else{
                if(!mousehack.set_constantFramePeriod(frameperiod)){
                    write_status("Error setting Frame Period");
                }
                else{
                    write_status("fixed frameperiod set");
                }
            }
        }
        //automatic
        else if(true)//!mousehack.automaticFramePeriodActive())
        {
            if(!mousehack.set_automaticFramePeriod()){
                write_status("Error setting frameperiod to auto mode");
            }
            else{
                write_status("set frame period auto");
            }
        }

}

void Gui::on_button_exp_fpmax_clicked()
{
        if(cb_exp_fpmax->get_active())
        {
            string value = entry_exp_fpmax->get_text();
            int fpmax = atoi(value.c_str());

            if(fpmax < 0x0E7E || fpmax > 0xFFFF){
                write_status("Error: fp max zu groß oder klein");
            }
            else{
                if(!mousehack.set_framePeriodMaxBound(fpmax)){
                    write_status("Error setting Frame Period Max Bound");
                }
                else{
                    write_status("Frame Period Max Bound set");
                }
            }
        }

        else
        {
            if(!mousehack.set_framePeriodMaxBound(12000)){
                write_status("Error setting Frame Period Max Bound to default value");
            }
        }
}

void Gui::on_button_exp_fpmin_clicked()
{
        if(cb_exp_fpmin->get_active())
        {
            string valuemin = entry_exp_fpmin->get_text();
            int fpmin = atoi(valuemin.c_str());

            if(fpmin < 0x0E7E || fpmin > 0xFFFF){
                write_status("Error: fp min zu groß oder klein");
                return;
            }

            if(!mousehack.set_framePeriodMinBound(fpmin)){
                write_status("Error setting min Frame Bound");
            }
            else{
                write_status("Frame Period Min Bound set");
            }
        }

        else
        {
            if(!mousehack.set_framePeriodMinBound(0x0e7e)){
                write_status("Error setting min Frame Bound to default value");}
        }
}

void Gui::on_button_exp_shmax_clicked()
{
        if(cb_exp_shmax->get_active())
        {
            string value = entry_exp_shmax->get_text();
            int shmax = atoi(value.c_str());

            if(!mousehack.set_shutterMaxBound(shmax)){
                write_status("Error setting shmax");
            }
            else{
                write_status("Shutter Max set");
            }
        }

        else
        {
            if(!mousehack.set_shutterMaxBound(0x2000)){
                write_status("Error setting Shutter Max to default value");
            }
        }
}

void Gui::on_button_exp_sh_clicked()
{
        if(cb_exp_sh->get_active())
        {
            string value = entry_exp_sh->get_text();
            int sh = atoi(value.c_str());

            if(!mousehack.set_constantShutter(sh)){
                write_status("Error setting constant Shutter");
            }
            else{
                write_status("Shutter set");
            }
        }

        else{
            if(!mousehack.set_automaticShutter()){
                write_status("Error setting shutter to auto mode");
            }
            else{
                write_status("set shutter mode auto");
            }
        }
}

void Gui::on_resetbutton_clicked()
{
        mousehack.reset();
        button_logic();
        write_status("Mousechip resetted");
}

void Gui::write_status(string status)
{
        cout << status << endl;
        statusbar->push(status);
}

void Gui::on_notebook1_switch_page()
{
        button_logic();
}

void Gui::on_tb_exp_all_toggled()
{
        if(tb_exp_all->get_active())
        {
            tb_exp_fp->set_active(true);
            tb_exp_fpmax->set_active(true);
            tb_exp_fpmin->set_active(true);
            tb_exp_sh->set_active(true);
            tb_exp_shmax->set_active(true);
            tb_exp_sq->set_active(true);
        }

        else
        {
            tb_exp_fp->set_active(false);
            tb_exp_fpmax->set_active(false);
            tb_exp_fpmin->set_active(false);
            tb_exp_sh->set_active(false);
            tb_exp_shmax->set_active(false);
            tb_exp_sq->set_active(false);
        }
}

void Gui::on_tb_connect_clicked()
{
    if(tb_connect_dev->get_active())
    {
        if(!mousehack.connect(deviceentry->get_text()))//baud_comboboxtext.get_active_text()
        {
            write_status("Error: Couldn't connect");
            tb_connect_dev->set_active(false);
        }

        else{
            write_status("Connected");
            tb_connect_id->set_sensitive(false);
        }
    }

    else{
        mousehack.disconnect();
        write_status("Disconnected");
        tb_connect_id->set_sensitive(true);
    }

    button_logic();
}

void Gui::on_tb_connect_id_clicked()
{
    if(tb_connect_id->get_active())
    {
        if(!mousehack.connect(atoi(identry->get_text().c_str())))
        {
            write_status("Error: Couldn't connect");
            tb_connect_id->set_active(false);
        }

        else{
            write_status("Connected");
            tb_connect_dev->set_sensitive(false);
        }
    }

    else{
        mousehack.disconnect();
        write_status("Disconnected");
        tb_connect_dev->set_sensitive(true);
    }

    button_logic();
}

void Gui::on_button_exp_setid_clicked()
{
        mousehack.setID(atoi(setidentry->get_text().c_str()));
        write_status("ID set");
}

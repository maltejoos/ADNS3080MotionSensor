#include <iostream>
#include "MotionSensor.h"

using namespace std;

int main()
{
    MotionSensor sensor;

    if(!sensor.connect("/dev/ttyUSB0")){
        cout << "Error connecting" << endl;
        return 0;
    }

    sensor.calibrate_using_image_region(0.05);

    sensor.resetMotion();

    //continuousMotion Mode
    /*
        sensor.init_continuousMotion();

        for(int i=0; i<100; i++)
        {
            sensor.read_continuousMotion();

            cout << sensor.get_dx() << " "
                 << sensor.get_dy() << " "
                 << sensor.get_vx() << " "
                 << sensor.get_vy() << endl;
        }

        sensor.stop_continuousMotion();
    */


    //integrateMotion Mode

        sensor.init_integrateMotion();

        for(int i=0; i<100; i++)
        {
            usleep(10000);//do something

            if(!sensor.read_integrateMotion()){
                cout << "Error: Motion Overflow" << endl;
                continue;
            }

                cout << sensor.get_dx() << " "
                     << sensor.get_dy() << " "
                     << sensor.get_vx() << " "
                     << sensor.get_vy() << endl;
        }

        sensor.stop_integrateMotion();

        sensor.disconnect();

    return 0;
}

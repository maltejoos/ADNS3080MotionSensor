#include <iostream>
#include "YRS.h"

using namespace std;

int main()
{
    YRS yrs(0.5, 0, 3.14);

	//Sensor with ID 0 is Master Sensor. Sensor with ID 1 is Slave Sensor.
    if(!yrs.connect(0,1)){
        return 0;}

    yrs.sensor1_obj()->calibrate_using_image_region(0.005);
    yrs.sensor2_obj()->calibrate_using_image_region(0.0045);

    yrs.init_computeMotion();

    for(int i=0; i<100; ++i)
    {
        if(!yrs.readMotion()){
            cout << "Error: Motion Overflow" << endl;
            continue;
        }

        cout << yrs.get_dx() << "\t"
        << yrs.get_dy() << "\t"
        << yrs.get_Vx() << "\t"
        << yrs.get_Vy() << "\t"
        << yrs.get_dYaw() << "\t"
        << yrs.get_Yawabs() << "\t"
        << yrs.get_YawRate() << endl;
    }

    yrs.stop_computeMotion();
    yrs.disconnect();

    return 0;
}

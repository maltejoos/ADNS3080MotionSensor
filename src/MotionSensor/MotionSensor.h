#include <SerialPort.h>
#include <iostream>
#include <sys/time.h>
#include <cmath>
#include <stdlib.h>
#include <stack>
#include <sstream>

#define version 1.0

#define x_framesize 		30
#define y_framesize 		30
#define framesize 	        x_framesize*y_framesize

using namespace std;

/**
MotionSensor interface class

After setting the sensor in a motion operating mode one has to do two steps to obtain motion data.
1) Recieve new motion data.
2) Read your relevant data.

That means, that you always have to recieve new motion data before reading any values. Otherwise you will just read old data. See also the example below.

There are three types of motion sensoring.

integrateMotion:
The sensor adds up motion data internally. When reading motion data it contains the results since last read. Make sure that the motion buffer did not overflow by checking the return value of \ref read_integrateMotion().

continuouseMotion:
The sensor sends motion data automatically. To obtain the current motion data you have to read it fast enough. Otherwise you will read old data from the serial stack.

computeMotion:
This mode computes the so called Motion Burst \ref motion_cycles times.
First, all motion data on the ADNS is resetted. Afterwards Motion Burst is executed \ref motion_cycles times. Then, new motion data is recieved and all relevant values are updated.
Be aware that motion data gets loss due to the implicated motion reset. Therefore the absolute motion values \ref xabs and \ref yabs will not be updated. Use continuouseMotion- or integrateMotion mode instead.

Example:

\code
#include "MotionSensor.h"

int main()
{
    MotionSensor sensor;
    sensor.connect("/dev/ttyUSB0");
    //sensor.easyConnect();

    sensor.calibrate_using_image_region(0.005);

    sensor.init_integrateMotion();

    //Read 100 times
    for(int i=0; i<100;++i)
    {
        if(!sensor.read_integrateMotion())
        {
            std::cout << "Motion Buffer overflowed. Recieved data is not valid" << std::endl;
            continue;
        }

        std::cout << sensor.get_dx() << " " << sensor.get_dy() << " " << sensor.get_vx() << " " << sensor.get_vy() << std::endl;
    }

    sensor.stop_integrateMotion();
    sensor.disconnect();

    return 0;
}
\endcode
*/
class MotionSensor
{
    private:

        SerialPort* serialport;
        string device;

        static vector<string> connectedDevices; //Store connected devices in this vector and skip them in connect(unsigned char id)
                                                //Neccessary because of buggy libSerial

        bool vecContainsString(vector<string>, string);


        //Frame
        unsigned char rgbimage[framesize*3];

        //Motion
        int dx_sum;
        int dy_sum;
        int xabs;
        int yabs;
        double vx;
        double vy;
        double vabs;
        double vaverage;
        double vaverage_standard_deviation;
        unsigned char motion_cycles; ///< Number of cycles Motion Burst is computed internally. Default is 200. You probably don't want to change it.
        unsigned char motion_status;
        unsigned char squal;
        unsigned char shutter_upper;
        unsigned char shutter_lower;
        unsigned char max_pixel;
        double counts_per_meter;
        timeval tv_ref;
        long double t1, t2;
        double delta_t; //in s
        int kalibration_counts;
        stack<double> vabs_history;
        bool createStatistics;

        int framePeriodMaxBoundIdeal; //store FramePeriodMaxBound value
                                      //alternative to get_framePeriodMaxBound()


    public:

        MotionSensor();

        double Version();

        bool motionBurst();
        void motionBurst_init();
        bool motionBurst_recv();

        void set_motionCycles(unsigned char);
        unsigned char get_motionCycles();

        bool connect(string);
        bool connect(unsigned char);
        bool disconnect();
        bool isConnected();

        //Frame
        int get_framesize();
        unsigned char* frameCaptureBurst();
        unsigned char* get_frame();
        int get_xFramesize();
        int get_yFramesize();

        //Motion
        bool computeMotion();
        void init_continuousMotion();
        void stop_continuousMotion();
        bool read_continuousMotion();
        void init_integrateMotion();
        void stop_integrateMotion();
        bool read_integrateMotion();
        int get_dx();
        int get_dy();
        int get_xabs();
        int get_yabs();
        double get_vx();
        double get_vy();
        double get_vabs();
        void resetMotion();
        void resetTranslation();
        void resetMotionStatistics();
        void resetMotionAll();
        void setStatisticsStatus(bool);
        bool getStatisticsStatus();
        void set_countsPerMeter(double);
        double get_countsPerMeter();
        double get_vAverage();
        double get_vAverageStandardDeviation();
        void clear_vAverage();

        //Kalibration
        void startCalibration();
        double stopCalibration(double);
        double calibrate_using_image_region(double);

        //Misc
        void setID(unsigned char);
        unsigned char getID();
        string getDevice();
        void reset();
        int read_register(unsigned char);
        void write_register(unsigned char, unsigned char);
        bool flash_firmware();
        bool firmware_flashed();
        unsigned int get_squal();

        unsigned int get_framePeriod();
        bool set_constantFramePeriod(int);
        bool set_automaticFramePeriod();
        bool automaticFramePeriodActive();

        unsigned int get_shutter();
        bool set_constantShutter(int);
        bool set_automaticShutter();
        bool automaticShutterActive();

        unsigned int get_framePeriodMaxBound();
        bool set_framePeriodMaxBound(int);

        unsigned int get_framePeriodMinBound();
        bool set_framePeriodMinBound(int);

        unsigned int get_shutterMaxBound();
        bool set_shutterMaxBound(int);
};

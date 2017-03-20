#include <iostream>
#include <cmath>
#include <string>
#include <sys/time.h>
#include <motion_sensor/MotionSensor.h>
//#include "../MotionSensor/MotionSensor.h"

#define PI 3.1415926535897932384626433832795

struct Point
{
    double x,y;
};

struct Line
{
    double xs,ys,xe,ye;
};

/**
YRS - Yaw Rate Sensor interface class

The Yaw Rate Sensor is composed of two Motion Sensors. One Motion Sensor is the Master Sensor and the other the Slave Sensor.
For correct motion and yaw angle/rate calculations it is neccessary to specify the translation and orientation of the Slave Sensor relative to the Master Sensor in coordinates of the Master Sensor.
In addition, both sensors has to be well calibrated. Use \ref sensor1_obj() (Master Sensor) and \ref sensor2_obj() (Slave Sensor) to access the underlying MotionSensor objects.

Any resulting motion is expressed in a virtual coordinate system (from now on called YRS coordinate system) in the center of a vectorial connection between both sensors. This coordinate system is always orientated in the same orientation as the Master Sensors coordinate system.
*/

class YRS
{
    private:

        MotionSensor sensor1;
        MotionSensor sensor2;

        Line l1; //beginning line
        Line l2; //line after delta t
        Point center1, center2;

        double distance_x; //in m
        double distance_y; //in m
        double torsion;
        long double t1, t2;
        double delta_t; //in s
        timeval tv_ref;

        double dx;
        double dy;
        double dYaw;
        double dTabs;
        double xabs;
        double yabs;
        double yawabs;

    public:

        YRS();
        YRS(double, double, double);
        YRS(double, double, double, string, string);
        YRS(double, double, double, unsigned char, unsigned char);

        static void transformTranslation(double&,double&,double,double,double,double,double,double);

        bool connect(string, string);
        bool connect(unsigned char, unsigned char);
        bool disconnect();
        bool isConnected();
        void set_relative_position(double, double, double);

        void init_computeMotion();
        void stop_computeMotion();
        bool readMotion();

        void resetMotion();

        double get_dYaw();
        double get_dx();
        double get_dy();
        double get_dTabs();

        double get_xabs();
        double get_yabs();
        double get_Yawabs();

        double get_YawRate();
        double get_Vx();
        double get_Vy();
        double get_Vabs();

        MotionSensor* sensor1_obj();
        MotionSensor* sensor2_obj();
};

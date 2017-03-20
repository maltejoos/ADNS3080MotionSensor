#include "YRS.h"

/**
Default Constructor
*/
YRS::YRS()
{
    l1.xs=l1.xe=l1.ys=l1.ye=l2.xs=l2.xe=l2.ys=l2.ye=0;
    distance_x=1;
    distance_y=1;
    dx=dy=xabs=yabs=yawabs=0;
    gettimeofday(&tv_ref, NULL);
}

/**
Constructor
\param d_x The absolute translation of the Slave Sensor relative to the Master Sensor in coordinates of the Master Sensor.
\param d_y The absolute translation of the Slave Sensor relative to the Master Sensor in coordinates of the Master Sensor.
\param angle The torison of the Slave Sensor relative to the Master Sensor.
*/
YRS::YRS(double d_x, double d_y, double angle)
{
    l1.xs=l1.xe=l1.ys=l1.ye=l2.xs=l2.xe=l2.ys=l2.ye=0;
    distance_x=fabs(d_x);
    distance_y=fabs(d_y);
    torsion=angle;
    dx=dy=xabs=yabs=yawabs=0;
    gettimeofday(&tv_ref, NULL);
}

/**
Open serial devices and connect to both sensors.
\param s1 The serial device of the Master Sensor.
\param s2 The serial device of the Slave Sensor.
\return true if connections could be established.
\return false if connections could not be established.
*/
bool YRS::connect(string s1, string s2)
{
    if(!sensor1.connect(s1)){
        cout << "Error connecting via " << s1 << endl;
        return false;}
    if(!sensor2.connect(s2)){
        cout << "Error connecting via " << s2 << endl;
        return false;}

    return true;
}

/**
Scan serial devices and connect both sensors using their ID.
\note The IDs of both sensors are not allowed to be equal.
\param id_master The Sensor ID of the Master Sensor.
\param id_slave The Sensor ID of the Slave Sensor.
\return true if connections could be established.
\return false if connections could not be established.
*/
bool YRS::connect(unsigned char id_master, unsigned char id_slave)
{
    if(id_master==id_slave)
    {
        cout << "Error: Sensors cannot have the same IDs!" << endl;
        return false;
    }

    if(!sensor1.connect(id_master)){
        return false;
    }
    if(!sensor2.connect(id_slave)){
        return false;
    }

    return true;
}

/**
Disconnect sensors and close serial ports.
\return true if sensors could be disconnected.
\return false if an error occured.
*/
bool YRS::disconnect()
{
    if(!sensor1.disconnect()){
        cout << "Error disconnecting Master Sensor" << endl;
        return false;}
    if(!sensor2.disconnect()){
        cout << "Error disconnecting Slave Sensor" << endl;
        return false;}

    return true;
}

/**
\return true if sensors are connected.
\return fale if sensors are not connected.
*/
bool YRS::isConnected()
{
    if(!sensor1.isConnected() || !sensor2.isConnected())
        return false;

    return true;
}

/**
Initialise Motion sensoring
*/
void YRS::init_computeMotion()
{
        timeval tv;

        gettimeofday(&tv, NULL);
        t1 = t2 = (tv.tv_sec-tv_ref.tv_sec) + tv.tv_usec/1000000.;

        sensor1.init_integrateMotion();
        sensor2.init_integrateMotion();


}

/**
Read new motion data.
\return true if recieved motion data is valid.
\return false if the motion buffer overflowed or a motion overflow occured during the Motion Burst.
*/
bool YRS::readMotion()
{
    timeval tv;

    bool ovf, ovftemp;

    ovf = sensor1.read_integrateMotion();
    ovftemp = sensor2.read_integrateMotion();

    ovf = ovf && ovftemp;

    t1=t2;

    gettimeofday(&tv, NULL);
    t2 = (tv.tv_sec-tv_ref.tv_sec) + tv.tv_usec/1000000.;

    delta_t = t2-t1;

//Schnittwinkel zweier Geraden

    l1.xs = 0;
    l1.ys = 0;
    l1.xe = distance_x;
    l1.ye = distance_y;

    l2.xs = sensor1.get_dx()/sensor1.get_countsPerMeter();
    l2.ys = sensor1.get_dy()/sensor1.get_countsPerMeter();

    //Rotationsmatrix
    //x_1=x_2*cos(torsion) - y_2*sin(torsion)
    //y_1=x_2*sin(torsion) + y_2*cos(torsion)
    l2.xe = distance_x+(sensor2.get_dx()*cos(torsion)-sensor2.get_dy()*sin(torsion))/sensor2.get_countsPerMeter();
    l2.ye = distance_y+(sensor2.get_dx()*sin(torsion)+sensor2.get_dy()*cos(torsion))/sensor2.get_countsPerMeter();

    center1.x = (l1.xe-l1.xs)/2.+l1.xs;
    center1.y = (l1.ye-l1.ys)/2.+l1.ys;
    center2.x = (l2.xe-l2.xs)/2.+l2.xs;
    center2.y = (l2.ye-l2.ys)/2.+l2.ys;

    dx=center2.x-center1.x;
    dy=center2.y-center1.y;
    dTabs=sqrt(pow(dx,2)+pow(dy,2));

//dYaw
    double scalprod, norm_l1, norm_l2, m1, m2, m1_enum, m1_denum, m2_enum, m2_denum;

    scalprod = (l1.xe-l1.xs)*(l2.xe-l2.xs)+(l1.ye-l1.ys)*(l2.ye-l2.ys);

    norm_l1 = sqrt(pow(l1.xe-l1.xs,2)+pow(l1.ye-l1.ys,2));
    norm_l2 = sqrt(pow(l2.xe-l2.xs,2)+pow(l2.ye-l2.ys,2));

    dYaw=acos(scalprod/norm_l1/norm_l2);

    //Vorzeichen
    m1_enum = l1.ye-l1.ys;
    m1_denum = l1.xe-l1.xs;

    if(m1_enum < 0 && m1_denum < 0){
        m1 = -m1_enum/m1_denum;}
    else{
        m1 = m1_enum/m1_denum;}


    m2_enum = l2.ye-l2.ys;
    m2_denum = l2.xe-l2.xs;

    if(m2_enum < 0 && m2_denum < 0){
        m2 = -m2_enum/m2_denum;}
    else{
        m2 = m2_enum/m2_denum;}


    if(m2<m1)//negativ
    {
        dYaw = -dYaw;
    }


//Least Square Schätzer (so nur im Spezialfall anwendbar)
/*
    dx = (sensor1.get_dx()/sensor1.get_countsPerMeter()+sensor2.get_dx()/sensor2.get_countsPerMeter())/2;
    dy = (sensor1.get_dy()/sensor1.get_countsPerMeter()+sensor2.get_dy()/sensor2.get_countsPerMeter())/2;

    dYaw = atan((sensor2.get_dy()/sensor2.get_countsPerMeter()-sensor1.get_dy()/sensor1.get_countsPerMeter())/(sensor2.get_dx()/sensor2.get_countsPerMeter()-sensor1.get_dx()/sensor1.get_countsPerMeter()+distance_x));
*/
    xabs+=dx;
    yabs+=dy;
    yawabs+=dYaw;

    return ovf;
}

/**
Stop Motion Sensoring
*/
void YRS::stop_computeMotion()
{
    sensor1.stop_integrateMotion();
    sensor2.stop_integrateMotion();

    l1.xs=l1.xe=l1.ys=l1.ye=l2.xs=l2.xe=l2.ys=l2.ye=0;
    dx=dy=xabs=yabs=yawabs=0;
}

/**
\return Delta yaw angle since last read [radian].
*/
double YRS::get_dYaw()
{
    return dYaw;
}

/**
\return Delta x movement [meter].
*/
double YRS::get_dx()
{
    return dx;
}

/**
\return Delta y movement [meter].
*/
double YRS::get_dy()
{
    return dy;
}

/**
\return Absolute movement [meter].
*/
double YRS::get_dTabs()
{
    return dTabs;
}

/**
\return Velocity in x direction [m/s].
*/
double YRS::get_Vx()
{
    return get_dx()/delta_t;
}

/**
\return Velocity in y direction [m/s].
*/
double YRS::get_Vy()
{
    return get_dy()/delta_t;
}

/**
\return Absolute velocity [m/s].
*/
double YRS::get_Vabs()
{
    return get_dTabs()/delta_t;
}

/**
\return Yaw rate [rad/s].
*/
double YRS::get_YawRate()
{
    return get_dYaw()/delta_t;
}

/**
\return Absolute x position [m].
*/
double YRS::get_xabs()
{
    return xabs;
}

/**
\return Absolute y position [m].
*/
double YRS::get_yabs()
{
    return yabs;
}

/**
\return Absolute yaw angle [rad].
*/
double YRS::get_Yawabs()
{
    return yawabs;
}

/**
\return Pointer to the underlying MotionSensor object of the the Master Sensor.
*/
MotionSensor* YRS::sensor1_obj()
{
    return &sensor1;
}

/**
\return Pointer to the underlying MotionSensor object of the Slave Sensor.
*/
MotionSensor* YRS::sensor2_obj()
{
    return &sensor2;
}

/**
Set the translation and orientation of the Slave Sensor relative to the Master Sensor.
\param d_x The translation of the Slave Sensor relative to the Master Sensor in coordinates of the Master Sensor.
\param d_y The translation of the Slave Sensor relative to the Master Sensor in coordinates of the Master Sensor.
\param angle The torison of the Slave Sensor relative to the Master Sensor.
*/
void YRS::set_relative_position(double d_x, double d_y, double angle)
{
    distance_x=d_x;
    distance_y=d_y;

    torsion=angle;
}

/**
Reset all motion values
*/
void YRS::resetMotion()
{
    sensor1.resetMotion();
    sensor2.resetMotion();

    l1.xs=l1.xe=l1.ys=l1.ye=l2.xs=l2.xe=l2.ys=l2.ye=0;
    dx=dy=xabs=yabs=yawabs=0;
}

/**
Transforms the meassured translation (expressed in a virtual coordinate system (YRS coordinate System) (cf. introduction)) in a translation in any other coordinate system.
\param transformed_dx Reference to the calculated new translation.
\param transformed_dy Reference to the calculated new translation.
\param translat_x The x position of the YRS coordinate system in coordinates of the desired coordinate system.
\param translat_y The y position of the YRS coordinate system in coordinates of the desired coordinate system.
\param torsion The orientation of the YRS coordinate system relative to the desired coordinate system.
\param dx The measured translation dx.
\param dy The measured translation dy.
\param dyaw The measured yaw angle dYaw.
\note The yaw angle in the transformed coordinate system is the same than the measured one.
*/
void YRS::transformTranslation(double& transformed_dx, double& transformed_dy, double translat_x, double translat_y, double torsion, double dx, double dy, double dyaw)
{
    transformed_dx = translat_x + dx*cos(torsion) - dy*sin(torsion) - (translat_x*cos(dyaw) - translat_y*sin(dyaw));
    transformed_dy = translat_y + dx*sin(torsion) + dy*cos(torsion) - (translat_x*sin(dyaw) + translat_y*cos(dyaw));
}

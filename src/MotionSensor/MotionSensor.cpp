#include "MotionSensor.h"

/**
Constructor
*/
MotionSensor::MotionSensor()
{
        //SerialPort
        serialport = new SerialPort("/dev/ttyUSB0");

        //Frame
        for(int i=0; i<framesize; ++i)
        {
            rgbimage[i*3] = 0xff;
            rgbimage[i*3+1] = 0xff;
            rgbimage[i*3+2] = 0xff;
        }

        //Motion
        xabs=yabs=vx=vy=vabs=motion_status=squal=shutter_upper=shutter_lower=vaverage=vaverage_standard_deviation=0;
        motion_cycles=200;
        createStatistics=false;
        counts_per_meter=0;
        t2=t1=0;
        framePeriodMaxBoundIdeal=12000;

        gettimeofday(&tv_ref, NULL);
}

/**
\return The Version of the library
*/
double MotionSensor::Version()
{
        return version;
}

/**
\return framesize in pixels
*/
int MotionSensor::get_framesize()
{
        return framesize;
}

/**
Execute Frame Capture and create a RGB image. The RGB image contains 900 pixles (\ref get_framesize()) where each pixel is represented by 3 bytes containig the RGB values.
\return A Pointer to the beginning of the RGB Image.
\warning Frame Capture Burst deletes the ADNS firmware. Motion sensoring is not available until firmware is flashed again (cf. \ref flash_firmware()).
*/
unsigned char* MotionSensor::frameCaptureBurst()
{
        //bild anfragen
        serialport->WriteByte('b');

        //empfange bild
        unsigned char temp;

        for(int i=0; i<framesize; ++i)
        {
            temp = serialport->ReadByte();
            temp = temp << 2;
            rgbimage[i*3] = temp;
            rgbimage[i*3+1] = temp;
            rgbimage[i*3+2] = temp;
        }

        return rgbimage;
}

/**
\return A Pointer to the beginning of the last created RGB image.
\warning No new image is captured. Use \ref frameCaptureBurst() instead.
*/
unsigned char* MotionSensor::get_frame()
{
        return rgbimage;
}

/**
\return X framesize in pixels
*/
int MotionSensor::get_xFramesize()
{
        return x_framesize;
}

/**
\return Y framesize in pixels
*/
int MotionSensor::get_yFramesize()
{
        return y_framesize;
}

/**
Execute Motion Burst and update relevant data.
First, all motion data on the ADNS is resetted. Afterwards Motion Burst is executed \ref motion_cycles times. Then, new motion data is recieved and all relevant values are updated (cf. note).
Use this function for pointwise measurement of motion.
\note Be aware that motion data gets loss due to the implicated motion reset. Therefore the absolute motion values \ref xabs and \ref yabs will not be updated. Use continuouseMotion- or integrateMotion mode instead.
*/
bool MotionSensor::computeMotion()
{
        resetMotion();

        timeval tv;

        gettimeofday(&tv, NULL);
        t1 = (tv.tv_sec-tv_ref.tv_sec) + tv.tv_usec/1000000.;

        if(!motionBurst()){
            return false;}

        gettimeofday(&tv, NULL);
        t2 = (tv.tv_sec-tv_ref.tv_sec) + tv.tv_usec/1000000.;

        delta_t = t2-t1;

        return true;
}

/**
Compute Burst Mode \ref motion_cycles times.
\ref dx_sum and \ref dy_sum are updated.
This method just calls \ref motionBurst_init() and \ref motionBurst_recv()
\warning This is a raw method for Motion Burst. It is recommended to use computeMotion- , continuousMotion- or integrateMotion mode instead.
\return true if recieved motion data is valid.
\return false if a motion overflow appeared during Motion Burst.
*/
bool MotionSensor::motionBurst()
{
        //burst mode
        motionBurst_init();
        return motionBurst_recv();

        //normal mode
        /*
        signed char dx, dy;

        serialstream << 'g';

        serialstream >> temp;
        serialstream >> dx;
        serialstream >> dy;


        if((temp & 0b00010000) == 0b00010000){ //  Motion Overflow
            cout << "Overflow" << endl;}

        else if((temp & 0b10000000) == 0b10000000)// MOT=1
        {
            if((int)dx!=0 || (int)dy!=0)
            {
                cout << "Delta X: " << (int)dx << "\t";
                cout << "Delta Y: " << (int)dy << endl;
                motion.add_motion((int)dx, (int)dy);
                motion.draw_arrow();
            }
        }
        */
}

/**
Start Burst Mode \ref motion_cycles times.
*/
void MotionSensor::motionBurst_init()
{
        serialport->WriteByte('h');
        serialport->WriteByte(motion_cycles);
}

/**
Recieve data and update \ref dx_sum and \ref dy_sum
\warning No velocities and no absolute motion data are calculated. Use computeMotion- , continuousMotion- or integrateMotion mode instead.
\return true if recieved motion data is valid.
\return false if a motion overflow appeared during Motion Burst.
*/
bool MotionSensor::motionBurst_recv()
{
        unsigned char upper,lower;

        motion_status = serialport->ReadByte();
        upper = serialport->ReadByte();
        lower = serialport->ReadByte();
        dx_sum = (int16_t)((upper << 8) + lower);
        upper = serialport->ReadByte();
        lower = serialport->ReadByte();
        dy_sum = (int16_t)((upper << 8) + lower);
        /*
        serialstream >> squal;
        serialstream >> shutter_upper;
        serialstream >> shutter_lower;
        serialstream >> max_pixel;
        */

        if((motion_status & 0b00010000) == 0b00010000){ //  Motion Overflow
                cout << "Motion Overflow (on ADNS)" << endl;
                return false;
        }

        return true;
}

/**
\return Delta x movement [counts].
*/
int MotionSensor::get_dx()
{
        return (int)dx_sum;
}

/**
\return Delta y movement [counts].
*/
int MotionSensor::get_dy()
{
        return (int)dy_sum;
}

/**
\return Absolute x position [counts].
\note Works only in integrateMotion mode or proper use of contiuouseMotion mode.
*/
int MotionSensor::get_xabs()
{
        return xabs;
}

/**
\return Absolute y position [counts].
\note Works only in integrateMotion mode or proper use of contiuouseMotion mode.
*/
int MotionSensor::get_yabs()
{
        return yabs;
}

/**
\return Velocity in x direction [m/s].
*/
double MotionSensor::get_vx()
{
        return ((double)dx_sum)/delta_t/counts_per_meter;
}

/**
\return Velocity in y direction [m/s].
*/
double MotionSensor::get_vy()
{
        return ((double)dy_sum)/delta_t/counts_per_meter;
}

/**
\return Absolute velocity [m/s].
*/
double MotionSensor::get_vabs()
{
        vabs = sqrt((double)(((int)dx_sum)*((int)dx_sum)+((int)dy_sum)*((int)dy_sum)))/delta_t/counts_per_meter;

        if(createStatistics)
        {
            //Durchschnitt
            vaverage = (vaverage+vabs)/2.0;

            //Standardabweichung
            vabs_history.push(vabs);
        }

        return vabs;
}

/**
\return Average velocity [m/s].
\note Motion statistics must be turned on (\ref setStatisticsStatus(bool)).
*/
double MotionSensor::get_vAverage()
{
        return vaverage;
}

/**
\return Standard deviation of velocity.
\note Motion statistics must be turned on (\ref setStatisticsStatus(bool)).
*/
double MotionSensor::get_vAverageStandardDeviation()
{
        double sum = 0;
        double fac = 1.0/(vabs_history.size()-1);

        while(!vabs_history.empty()){
            sum+=(vabs_history.top()-vaverage)*(vabs_history.top()-vaverage);
            vabs_history.pop();
        }

        vaverage_standard_deviation = sqrt(fac*sum);

        return vaverage_standard_deviation;
}

/**
Clear average velocity.
*/
void MotionSensor::clear_vAverage()
{
        vaverage=0;
        while(!vabs_history.empty()){
            vabs_history.pop();}
}

/**
Read a register on the ADNS.
\param reg The address of the register to be read.
\return The value of the read register.
*/
int MotionSensor::read_register(unsigned char reg)
{
        unsigned char temp;
        serialport->WriteByte('f');
        serialport->WriteByte(reg);
        temp = serialport->ReadByte();

        return (int)temp;
}

/**
Write a register on the ADNS.
\param adr The address of the register.
\param value The value to be written.
*/
void MotionSensor::write_register(unsigned char adr, unsigned char value)
{
        serialport->WriteByte('l');
        serialport->WriteByte(adr);
        serialport->WriteByte(value);

        char temp;
        temp = serialport->ReadByte();
}

/**
Flash the firmware to the ADNS.
\return true if flashing was successfull.
\return false if a CRC error occured.
*/
bool MotionSensor::flash_firmware()
{
        serialport->WriteByte('d');
        unsigned char checksum1, checksum2;

        checksum1 = serialport->ReadByte();
        checksum2 = serialport->ReadByte();

        if(checksum1 != 0xEF || checksum2 != 0xBE)
        {
            cout << "Error: CRC Test failed!" << endl;
            return false;
        }

        else
        {
            return true;
        }
}

/**
Reset the ADNS
*/
void MotionSensor::reset()
{
        serialport->WriteByte('j');
}

/**
Set the calibration factor \ref counts_per_meter.
\param value The new value.
*/
void MotionSensor::set_countsPerMeter(double value)
{
        counts_per_meter = value;
}

/**
\return The calibration factor \ref counts_per_meter
*/
double MotionSensor::get_countsPerMeter()
{
        return counts_per_meter;
}

/**
Reset motion on the ADNS
*/
void MotionSensor::resetMotion()
{
        serialport->WriteByte('i');
        char temp;
        temp = serialport->ReadByte(); //Warten bis Motion Clear durchgeführt
        dx_sum=dy_sum=0;
}

/**
Reset internally added absolute positions
*/
void MotionSensor::resetTranslation()
{
        xabs=yabs=dx_sum=dy_sum=0;
}

/**
Reset motion statistics
*/
void MotionSensor::resetMotionStatistics()
{
        clear_vAverage();
        vaverage=vaverage_standard_deviation=0;
}

/**
Calls \ref resetMotion(), \ref resetTranslation() and \ref resetMotionStatistics().
*/
void MotionSensor::resetMotionAll()
{
        resetMotion();
        resetTranslation();
        resetMotionStatistics();
}

/**
Start calibration (distance mode).
Counts are internally added up until \ref stopCalibration() is called. The meanwhile absolute movement is used to calculate the calibration factor.
\note It is recommended to use \ref calibrate_using_image_region(double) instead.
*/
void MotionSensor::startCalibration()
{
        resetMotionAll();
        kalibration_counts = 0;
        init_integrateMotion();
}

/**
Stop the calibration (distance mode).
The calibration factor is calculated by the actual driven distance and the counted value.
\param distance The actual driven distance in meter [m].
\return The calculated calibration factor.
\note It is recommended to use \ref calibrate_using_image_region(double) instead.
*/
double MotionSensor::stopCalibration(double distance)
{
        stop_integrateMotion();
        kalibration_counts = sqrt(xabs*xabs+yabs*yabs);
        counts_per_meter = kalibration_counts/distance;
        resetMotionAll();
        return counts_per_meter;
}

/**
Calculate the calibration factor using the size of the imaged surface.
\param d The side length of the captured image region in meter [m].
\return the caluclated calibration factor
*/
double MotionSensor::calibrate_using_image_region(double d)
{
        counts_per_meter = 30/d;
        return counts_per_meter;
}

/**
Activate/deactivate motion statistics.
Activate motion statistics before using \ref get_vAverage() or \ref get_vAverageStandardDeviation().
\param set Activate or deactivate motion statistics.
*/
void MotionSensor::setStatisticsStatus(bool set)
{
    createStatistics=set;

    if(!createStatistics){
        resetMotionStatistics();
    }
}

/**
\return motion statistic status.
*/
bool MotionSensor::getStatisticsStatus()
{
    return createStatistics;
}

/**
Get status of ADNS firmware. For motion sensoring the firmware has to be flashed!
\return true if firmware is flashed.
\return false if not.
*/
bool MotionSensor::firmware_flashed()
{
        read_register(0x1f); //Beim ersten lesen kommt oft müll
        return (read_register(0x1f) == 81);
}

/**
\return The currently used frame period [clock cycles].
*/
unsigned int MotionSensor::get_framePeriod()
{
        unsigned char upper, lower;
        upper = read_register(0x11); //first upper
        lower = read_register(0x10); //then lower
        return (upper << 8) + lower;
}


/**
Set the frame period to a constant value.
\param period The (constant) frame period [in clock cycles]. The value must be between 3710 and 12000.
\note In addition, the three bound registers must also follow this rule when set to non-default values: frame period max bound >= frame period min bound + shutter max bound.
\return true if setting frame period was successfull.
\return false if an error occured.
*/
bool MotionSensor::set_constantFramePeriod(int period)
{
        if(period < 0x0E7E || period > 12000){
            cout << "Error: frame period not in valid range" << endl;
            return false;}

        unsigned char ext_conf = read_register(0x0b);
        if((ext_conf & 0b10000000) == 0x00)//Busy Bit == 0 --> writing allowed
        {

            unsigned char upper, lower, ext_value;
            lower = (period & 0xff);
            upper = (period >> 8) & 0xff;

            ext_value = ext_conf | 0b00000001; //Bit 0 auf 1 setzen. set fixed framerate = 1 ---> manually

            write_register(0x0b, ext_value);

            set_framePeriodMaxBound(period);
        }
        else{
            cout << "Error: Busy Bit set" << endl;
            return false;
        }

        return true;
}

/**
Set the frame period to automatic mode.
\return true if setting frame period to auto mode was successfull.
\return false if an error occured.
*/
bool MotionSensor::set_automaticFramePeriod()
{
        unsigned char ext_conf = read_register(0x0b);

        if((ext_conf & 0b10000000) == 0x00)//Busy Bit == 0 --> writing allowed
        {
            unsigned char ext_value = ext_conf & 0b11111110; // Bit 0 auf 0 setzen (framerate automatically)
            write_register(0x0b, ext_value);

            //wait until new settings take effect
            while((read_register(0x0b) & 0b10000000) != 0x00){}

        }
        else{
                cout << "Error: Busy Bit set" << endl;
                return false;
        }

        return true;
}

/**
\return The upper bound of the frame period.
*/
unsigned int MotionSensor::get_framePeriodMaxBound()
{
        unsigned char upper, lower;
        upper = read_register(0x1A);
        lower = read_register(0x19);
        return (upper << 8)+lower;
}

/**
\return The lower bound of the frame period.
*/
unsigned int MotionSensor::get_framePeriodMinBound()
{
        unsigned char upper, lower;
        upper = read_register(0x1C);
        lower = read_register(0x1B);
        return (upper << 8)+lower;
}

/**
\return The upper bound of the shutter speed.
*/
unsigned int MotionSensor::get_shutterMaxBound()
{
        unsigned char upper, lower;
        upper = read_register(0x1E);
        lower = read_register(0x1D);
        return (upper << 8)+lower;
}

/**
\return true if the sensor sets the frame period automatically.
\return false if the frame period is set to a constant value.
*/
bool MotionSensor::automaticFramePeriodActive()
{
        unsigned char ext_conf = read_register(0x0b);

        return ((ext_conf | 0b11111110) == 0b11111110);
}

/**
\return true if the sensor sets the shutter speed automatically.
\return false if the shutter speed is set to a constant.
*/
bool MotionSensor::automaticShutterActive()
{
        unsigned char ext_conf = read_register(0x0b);

        return ((ext_conf | 0b11111101) == 0b11111101);
}

/**
\return The SQUAL-Register value. A quantity for the imaged surface quality.
*/
unsigned int MotionSensor::get_squal()
{
        return (unsigned int) read_register(0x05);
}

/**
Set a constant shutter speed.
\param sh The constant shutter speed [in clock cycles].
\note Because the automatic frame rate feature is related to the shutter value it is also set to an appropriate constant value. However one can set the frame rate to another constant value.
\note In addition, the three bound registers must also follow this rule when set to non-default values: frame period max bound >= frame period min bound + shutter max bound.
\return true if setting was successfull.
\return false if an error occured.
*/
bool MotionSensor::set_constantShutter(int sh)
{
        int fpmax = get_framePeriodMaxBound();

        unsigned char ext_conf = read_register(0x0b);
        if((ext_conf & 0b10000000) == 0x00)//Busy Bit == 0 --> writing allowed
        {
            unsigned char ext_value, sh_lower, sh_upper, fpmax_lower, fpmax_upper;

            ext_value = ext_conf | 0b00000010; //Bit 1 auf 1 setzen. AGC disabled

            write_register(0x0b, ext_value); //disbale AGC in extended config

            //wait until new settings take effect
            while((read_register(0x0b) & 0b10000000) != 0x00){}

            sh_lower = (sh & 0xff);
            sh_upper = (sh >> 8) & 0xff;
            write_register(0x1d, sh_lower); //first shutter max
            write_register(0x1e, sh_upper);

            set_framePeriodMaxBound(fpmax); //then fp max bound
        }

        else{
            cout << "Error: Busy Bit set" << endl;
            return false;
        }

        return true;
}

/**
\return The current shutter speed [clock cycles].
*/
unsigned int MotionSensor::get_shutter()
{
        unsigned char upper, lower;
        upper = read_register(0x0f);
        lower = read_register(0x0e);
        return (upper << 8)+lower;
}

/**
Set shutter speed to auto mode.
\return true if setting shutter speed to auto mode was successfull.
\return false if an error occured.
*/
bool MotionSensor::set_automaticShutter()
{
        unsigned char ext_conf = read_register(0x0b);

        if((ext_conf & 0b10000000) == 0x00)//Busy Bit == 0 --> writing allowed
        {
            unsigned char ext_value = ext_conf & 0b11111101; // Bit 1 auf 0 setzen (AGC enabled)
            write_register(0x0b, ext_value);

            //wait until new settings take effect
            while((read_register(0x0b) & 0b10000000) != 0x00){}

        }
        else{
                cout << "Error: Busy Bit set" << endl;
                return false;
        }

        return true;
}

/**
Set upper frame period bound.
\param fpmax The upper frame period bound [in clock cycles]. The value must be between 3710 and 12000.
\return true if setting the upper frame period bound was successfull.
\return flase if an error occured.
*/
bool MotionSensor::set_framePeriodMaxBound(int fpmax)
{
        if(fpmax < 0x0E7E || fpmax > 12000){
            cout << "Error: frame period not in valid range" << endl;
            return false;
        }

        unsigned char fpmax_upper, fpmax_lower;

        fpmax_lower = (fpmax & 0xff);
        fpmax_upper = (fpmax >> 8) & 0xff;
        write_register(0x19, fpmax_lower);
        write_register(0x1a, fpmax_upper);

        //wait until new settings take effect
        while((read_register(0x0b) & 0b10000000) != 0x00){}

        framePeriodMaxBoundIdeal = fpmax;

        return true;
}

/**
Set lower frame period bound.
\param fpmin The lower frame period bound [in clock cycles]. The value must be between 3710 and 12000.
\note In addition, the three bound registers must also follow this rule when set to non-default values: frame period max bound >= frame period min bound + shutter max bound.
\return true if setting was successfull.
\return false if an error occured.
*/
bool MotionSensor::set_framePeriodMinBound(int fpmin)
{
        int fpmax = get_framePeriodMaxBound();

        if(fpmin < 0x0E7E || fpmin > 12000){
            cout << "Error: frame period not in valid range" << endl;
            return false;
        }

        unsigned char fpmin_upper, fpmin_lower;

        fpmin_lower = (fpmin & 0xff);
        fpmin_upper = (fpmin >> 8) & 0xff;
        write_register(0x1b, fpmin_lower); //first min bound
        write_register(0x1c, fpmin_upper);

        return set_framePeriodMaxBound(fpmax); //then frame period max bound
}

/**
Set upper shutter speed bound.
\param shmax The upper shutter speed bound [in clock cycles].
\note In addition, the three bound registers must also follow this rule when set to non-default values: frame period max bound >= frame period min bound + shutter max bound.
\return true if setting was successfull.
\return false if an error occured.
*/
bool MotionSensor::set_shutterMaxBound(int shmax)
{
        int fpmax= get_framePeriodMaxBound();

        unsigned char shmax_upper, shmax_lower;

        shmax_lower = (shmax & 0xff);
        shmax_upper = (shmax >> 8) & 0xff;
        write_register(0x1d, shmax_lower); //first max bound
        write_register(0x1e, shmax_upper);

        return set_framePeriodMaxBound(fpmax); //then frame period max bound
}

/**
Open serial device and connect to the sensor.
\param dev The serial device the sensor is connected to.
\return true if connection could be etablished.
\return false if connection could not be established.
*/
bool MotionSensor::connect(string dev)
{
        //placement new
        serialport = new(serialport) SerialPort(dev);

        try{
            serialport->Open();
        }
        catch(std::exception& excp){
            cout << excp.what() << endl << endl;
            return false;
        }


        if(serialport->IsOpen())
        {
            //setze parameter (8N1)
            serialport->SetBaudRate(SerialPort::BAUD_57600);
            serialport->SetCharSize(SerialPort::CHAR_SIZE_8);
            serialport->SetNumOfStopBits(SerialPort::STOP_BITS_1);
            serialport->SetParity(SerialPort::PARITY_NONE);
        }

        if(!isConnected())
        {
            cout << "Error connecting to Microcontroller" << endl;
            serialport->Close();
            return false;
        }

        device = dev;
        connectedDevices.push_back(device);

        return true;
}

/**
Scan serial devices and connect to the sensor with specific ID.
\param id The Sensor ID one want to connect.
\return true if connection could be etablished.
\return false if connection could not be established.
*/
bool MotionSensor::connect(unsigned char id)
{
    cout << "Scanning devices for Sensor with ID: " << (int)id << " ...";

    for(int i=0; i<100; ++i)
    {
        string dev_base = "/dev/ttyUSB";
        stringstream dev;
        dev << dev_base << i;

        if(vecContainsString(connectedDevices, dev.str())){
            continue;
        }

		//placement new
		serialport = new(serialport) SerialPort(dev.str());

        try{
            serialport->Open();
        }
        catch(SerialPort::OpenFailed){
            //cout << "Error opening device" << dev.str() << endl;
            continue;
        }
        catch(SerialPort::AlreadyOpen){
            continue;
        }

        if(serialport->IsOpen())
        {
            //setze parameter (8N1)
            serialport->SetBaudRate(SerialPort::BAUD_57600);
            serialport->SetCharSize(SerialPort::CHAR_SIZE_8);
            serialport->SetNumOfStopBits(SerialPort::STOP_BITS_1);
            serialport->SetParity(SerialPort::PARITY_NONE);

            if(isConnected()){
                if(getID() == id){
                    cout << " done. Sensor connected to " << dev.str() << endl;
                    device = dev.str();
                    connectedDevices.push_back(device);
                    return true;
                }
                else{
                    serialport->Close();
                }
            }
            else{
                serialport->Close();
            }
        }
    }

    cout << " done. Error: No sensor found!" << endl;

    return false;
}

/**
Disconnect from the sensor and close serial device.
\return true if sensor is disconnected
\return false if an error occured.
*/
bool MotionSensor::disconnect()
{
        if(serialport->IsOpen())
        {
            serialport->Close();

            for(int i=0; i<connectedDevices.size(); ++i){
                if(connectedDevices[i] == device){
                    connectedDevices.erase(connectedDevices.begin()+i);
                }
            }
        }

        return !serialport->IsOpen();
}

/**
\return true if sensor is connected.
\return fale if sensor is not connected.
*/
bool MotionSensor::isConnected()
{
        if(serialport->IsOpen())
        {
            //first of all, stop any motion operation in case any is running
            serialport->WriteByte('1');
            serialport->WriteByte('3');

            //Stopped integrateMotion would send 5 bytes now
            try
            {
                serialport->ReadByte(100);
                serialport->ReadByte(100);
                serialport->ReadByte(100);
                serialport->ReadByte(100);
                serialport->ReadByte(100);
            }
            catch(SerialPort::ReadTimeout){}

            unsigned char uctest;
            serialport->WriteByte('o');

            try{
                uctest = serialport->ReadByte(200);
            }
            catch(SerialPort::ReadTimeout){
                return false;
            }

            if(uctest != 42)
            {
                    return false;
            }

            //erste zeichen nach neustart oft undefiniert ---> abfangen
            read_register(0x00);

            return (read_register(0x00) == 0x17);
        }

        return false;
}

/**
Set sensor in continuous motion operation mode. The sensor will continuously MotionBurst \ref motion_cycles times and send the result until \ref stop_continuousMotion() is called.
Make sure to call \ref read_continuousMotion() fast enough to obtain current data.
*/
void MotionSensor::init_continuousMotion()
{
        resetMotion();

        serialport->WriteByte('m');
        serialport->WriteByte(motion_cycles);

        timeval tv;
        gettimeofday(&tv, NULL);
        t1 = (tv.tv_sec-tv_ref.tv_sec) + tv.tv_usec/1000000.;
}

/**
Read motion data when operating in continuouse motion mode. Recieved motion data is the summed result of the \ref motion_cycles Motion Bursts.
Make sure to call this methode fast enough to obtain current data.
\return true if recieved motion data is valid.
\return false if a motion overflow occured during the Motion Burst (overflow on the ADNS3080).
*/
bool MotionSensor::read_continuousMotion()
{
        timeval tv;
        gettimeofday(&tv, NULL);
        t2 = (tv.tv_sec-tv_ref.tv_sec) + tv.tv_usec/1000000.;

        delta_t=t2-t1;
        t1=t2;

        bool ovf = motionBurst_recv();

        xabs+=dx_sum;
        yabs+=dy_sum;

        return ovf;
}

/**
Stop continuouse motion operation mode.
*/
void MotionSensor::stop_continuousMotion()
{
        serialport->WriteByte('1');
        resetMotion();
}

/**
Set sensor in integrate motion mode. Motion data will be internally summed until \ref read_integrateMotion() is called.
*/
void MotionSensor::init_integrateMotion()
{
        resetMotion();

        serialport->WriteByte('n');

        timeval tv;
        gettimeofday(&tv, NULL);
        t1 = (tv.tv_sec-tv_ref.tv_sec) + tv.tv_usec/1000000.;
}

/**
Read motion data when operating in integrate motion mode. The recieved motion data is the summed result since last read.
\return true if recieved motion data is valid.
\return false if the internal motion buffer overflowed or a motion overflow occured during the Motion Burst (overflow on the ADNS3080).
*/
bool MotionSensor::read_integrateMotion()
{
        timeval tv;
        gettimeofday(&tv, NULL);
        t2 = (tv.tv_sec-tv_ref.tv_sec) + tv.tv_usec/1000000.;

        delta_t=t2-t1;
        t1=t2;

        serialport->WriteByte('2');
        bool ovf = motionBurst_recv();

        if((motion_status & 0b00000100) == 0b00000100)
        {
                cout << "integrate Motion Overflow" << endl;
                return false;
        }

        if(!ovf)
        {
                return false;
        }

        xabs+=dx_sum;
        yabs+=dy_sum;

        return true;
}

/**
Stop integrate motion operation mode.
*/
void MotionSensor::stop_integrateMotion()
{
        serialport->WriteByte('3');

        motionBurst_recv();

        xabs+=dx_sum;
        yabs+=dy_sum;

        resetMotion();
}

/**
Set \ref motion_cycles. Default is 200. You probably don't want to change it.
\param cycles Number of cycles Motion Burst shall be computed internally.
*/
void MotionSensor::set_motionCycles(unsigned char cycles){
        motion_cycles=cycles;
}

/**
\return \ref motion_cycles.
*/
unsigned char MotionSensor::get_motionCycles()
{
        return motion_cycles;
}

/**
Read specific Sensor ID. If never set, the ID is 0.
\return specific sensor ID.
*/
unsigned char MotionSensor::getID()
{
        serialport->WriteByte('q');
        try{
            return serialport->ReadByte(200);
        }
        catch(SerialPort::ReadTimeout)
        {
            return 0;
        }
}

/**
Set specific Sensor ID.
\param id The id to be set.
*/
void MotionSensor::setID(unsigned char id)
{
        serialport->WriteByte('p');
        serialport->WriteByte(id);
}

/**
Get the device the sensor is connected to.
\return the device as a string.
*/
string MotionSensor::getDevice()
{
        return device;
}

vector<string> MotionSensor::connectedDevices;

bool MotionSensor::vecContainsString(vector<string> vec, string s)
{
        for(int i=0; i<vec.size(); ++i){
            if(vec[i] == s){
                return true;
            }
        }

        return false;
}

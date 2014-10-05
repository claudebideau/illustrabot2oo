/*!
 * \file ms_element.h
 * \author claude.bideau@gmail.com
 * \version 0.1
 * \brief This module provides a motor sensor element declaration.
 * \date  2014/05/01
 * 
 * \section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This module provides a common declaration.
 *
 */
#ifndef __MS_ELEMENT_H
#define __MS_ELEMENT_H

/**   1. Standard Library Include                                   **/
#include <iostream>
#include <sstream>
#include <string>
#ifdef ILLUSOO_THREAD 
#include <thread>
#include <mutex>
#endif

/**   1. Include files  (own)                                       **/
#include "gpio.h"
#include "easydrv.h"
#include "inireader.h"
#include "rttrace.h"
#include "angle.h"

using namespace std;

/**   2a.   External Functions                                      **/
/**   2b.   External Data                                           **/
/**   3.  Type Definitions                                          **/
/**   5.  Local Functions                                           **/
/**   6.  Macros / Defines                                          **/

#define MOTOR_CALIBRATION 0x000002
#define MOTOR_STEP        0x000004


typedef enum eMState {MS_INIT=0, MS_UNCALIBRATE, MS_READY } teMSState;


 /**
 * \class MotorSensorElementCl
 * \brief 
 *
 * basic Angle motor:
 * 
 *     sensor : position 0°
 *     angle range : + x° /- y°     ( to be determine )
 *     step range  : + X  /- Y step ( to be determine )
 *     
 * 
 * wakeup   -> turn off sleep IO
 * sleep    -> turn on  sleep IO
 * 
 * enable   ->  activate enable IO
 * disable  ->  desactivate enable IO
 * 
 * 
 * 
 * operating action :
 * - calibrate() => find the 0°
 * - speed(unsigned int)  => set stepper speed
 * - set(int)    => provide new angle : need to determine the range of int
 * - rise() => thread api : step rising
 * - fall() => thread api : step falling
 * 
 * monitor action:
 * - sensor()  => return current value(s)
 * - current() => return current angle value
 * - dir()     => return current direction
 * - speed()   => return current speed value
 * 
 * debug interface
 * - ini(string,int) => ini parameter change
 * - set_dir(int) => use only in manual
 * - do_step()    => use only in manual
 * 
 */
class MotorSensorElementCl
{
    public:
        MotorSensorElementCl( iniCl *, std::string);
        std::string name();
        /* setting */
        void wakeup();
        void sleep();
        void enable();
        void disable();
        teMSState state();
        unsigned int speed(unsigned int);
        unsigned int speed(void);
        unsigned int speed_mask(void);
        bool calibrate(void);
        // void attach(MotorSensorElementCl *);

        /* operating action */
        void set(int);
        void rise(void);
        void fall(void);

        /* monitor   action */
        void sensor(unsigned char *);
        void current(int);
        int current();
        int dir(void);
        int min();
        void min(int);
        void min(int, int);
        void min(int *, int *);
        int max();
        void max(int);
        void max(int, int);
        void max(int *, int *);

        void factor(int *);
        
        /* debug     action */
        void ini(std::string, int);
        void dir(int);
        void do_step();

        virtual ~MotorSensorElementCl();
        
        friend ostream &operator<<( ostream &output, 
                                       MotorSensorElementCl &MS )
        { 
            // unsigned int L_u32speed = MS.speed();
            std::string L_sensorName;
            ostringstream L_idSensor;   // stream used for the conversion

            output << "[" << MS._name << " ]" << endl;
            output << "type=" << "MotorSensorElementCl" << endl;
            output << "driver=" << MS._pDriver->name() << endl;
            if (MS._pSensor != NULL)
            {
                // L_sensorName = MS._name + "_" ;
                L_idSensor << MS._pSensor->id;             
                L_sensorName = MS._name + "_" + L_idSensor.str();
                output << "sensor=" << L_sensorName << endl;
                
            }
            // output << "sensor=" << "MotorSensorElementCl" << endl;
            output << "min=" << MS._minStep << " " << MS._minAngle << endl;
            output << "max=" << MS._maxStep << " " << MS._maxAngle << endl;
            output << "calibration=" << MS._calibrationAngle << endl;
            output << "speed=" << MS.speed() << endl <<endl ;
            
            output << *MS._pDriver ;

            if (MS._pSensor != NULL)
            {
                output << "[" << L_sensorName << "]" << endl;
                output << "input=" << MS._pSensor->id << endl << endl;
                
            }
            
            return output;            
        }
    protected:
    private:

        void _anglePreProc(void);
        
        teMSState       _state;
        std::string     _name;
        int             _minStep;
        int             _minAngle;
        int             _maxStep;
        int             _maxAngle;
        unsigned int    _factorStep;
        int             _calibrationStep;
        int             _calibrationAngle;
        
        int             _currentStep;
        int             _requireStep;
        int             _direction;
        int             _speed;
        EasyDriverCl  * _pDriver;
        GpioInCl      * _pSensor;
        RtTrace       * _trace;

        /* calibration */
        bool            _bDetectEdge;
        teValue         _teValueCurrent;
        teValue         _teValuePrev;
        
};



extern std::map<std::string, MotorSensorElementCl *> G_MapElementObj;


#endif // __MS_ELEMENT_H

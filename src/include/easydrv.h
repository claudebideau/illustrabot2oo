/*!
 * \file easydrv.h
 * \author claude.bideau@gmail.com
 * \version 0.1
 * \brief This module provides a easyDrv declaration.
 * \date  2014/03/21
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
#ifndef EASYDRV_H
#define EASYDRV_H

/**   1. Standard Library Include                                   **/
#include <iostream>
#include <sstream>
#include <string>
#include <list>

/**   1. Include files  (own)                                       **/
#include "gpio.h"
#include "inireader.h"

/**   2a.   External Functions                                      **/
/**   2b.   External Data                                           **/
/**   3.  Type Definitions                                          **/
/**   5.  Local Functions                                           **/
/**   6.  Macros / Defines                                          **/

// defines for motor direction
#define CLOCKWISE     LOW
#define ANTICLOCKWISE HIGH

/**
 * \enum eEDState
 * \brief ED state value
 *
 *  - ED_CLOSE      : the element is undefined or close
 *  - ED_READY      : the element is ready
 */
typedef enum eEdState {ED_INIT=0, ED_READY } teEdState;

typedef enum eEdIOType {STEP=0, DIR, ENABLE, M0, M1, M2, SLEEP} teEdIOType;

const std::string GPIOKEYS[] =  {"step", "dir", "enable", "m0", "m1", "m2", "sleep"};

 /**
 * \class GpioCl
 * \brief 
 *
 */
class EasyDriverCl
{
    public:
        EasyDriverCl();
		EasyDriverCl(iniCl *, std::string );
        void attach(teEdIOType, GpioOutCl *);
        void unattach(teEdIOType);
        void enable();
        void disable();
        void sleep();
        void wakeup();
        void step_pulse(int);
        void step_fall();
        teValue dir(void);
        void setSpeed(unsigned int);
        virtual ~EasyDriverCl();
        inline std::string name() { return _name; }
        unsigned int speed;

        friend ostream &operator<<( ostream &output, 
                                    EasyDriverCl &ED )
        { 
            output << "[" << ED._name << " ]" << endl;
            if (ED.io[ENABLE] != NULL) output << "enable="<< ED.io[ENABLE]->id << endl;
            if (ED.io[SLEEP ] != NULL) output << "sleep=" << ED.io[SLEEP ]->id << endl;
            if (ED.io[M0    ] != NULL) output << "m0="    << ED.io[M0    ]->id << endl;
            if (ED.io[M1    ] != NULL) output << "m1="    << ED.io[M1    ]->id << endl;
            if (ED.io[M2    ] != NULL) output << "m2="    << ED.io[M2    ]->id << endl;
            if (ED.io[STEP  ] != NULL) output << "step="  << ED.io[STEP  ]->id << endl;
            if (ED.io[DIR   ] != NULL) output << "dir="   << ED.io[DIR   ]->id << endl;

            output << endl;
            
            return output;            
        }        
    protected:
    private:
		std::string  _name;
        teEdState    state;
        teValue      currentDir;
        GpioOutCl  * io[7];
};

#endif // EASYDRV_H

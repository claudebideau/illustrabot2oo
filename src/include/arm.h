/*!
 * \file arm.h
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
#ifndef ARM_H
#define ARM_H

/**   1. Standard Library Include                                   **/
#include <iostream>
#include <sstream>
#include <string>
#include <list>

/**   1. Include files  (own)                                       **/
#include "ms_element.h"
#include "inireader.h"

/**   2a.   External Functions                                      **/
/**   2b.   External Data                                           **/
/**   3.  Type Definitions                                          **/
/**   5.  Local Functions                                           **/
/**   6.  Macros / Defines                                          **/

/**
 * \enum eArmState
 * \brief Arm state value
 *
 *  - ARM_CLOSE      : the element is undefined or close
 *  - ARM_READY      : the element is ready
 */
typedef enum eArmState {ARM_INIT=0, ARM_READY } teArmState;

typedef enum eArmType {SHOULDER=0, ARM, FOREARM} teArmType;

typedef struct sArmPosition {
    int longitude;
    int latitude;
    int radius;
} tsArmPosition;
    
 /**
 * \class ArmCl
 * \brief 
 *
 */
class ArmCl
{
    public:
		ArmCl(iniCl *, std::string );
        void attach(teArmType, MotorSensorElementCl *);
        void unattach(teArmType);
        teArmState state();
        void enable();
        void disable();
        void sleep();
        void wakeup();
        void set(tsArmPosition);
        void get(tsArmPosition * );
        void rise(void);
        void fall(void);        
        bool calibrate(void);

        virtual ~ArmCl();
        inline std::string name() { return _name; }
        unsigned int speed;
        unsigned int speed_mask;

        friend ostream &operator<<( ostream &output, 
                                    ArmCl &ArmObj )
        { 
            unsigned char L_u8Idx;
            output << "[" << ArmObj._name << " ]" << endl;
            if (ArmObj._Elt[SHOULDER] != NULL) output << "shoulder=" << ArmObj._Elt[SHOULDER]->name() << endl;
            if (ArmObj._Elt[ARM] != NULL)      output << "arm="      << ArmObj._Elt[ARM]->name()      << endl;
            if (ArmObj._Elt[FOREARM] != NULL)  output << "forearm="  << ArmObj._Elt[FOREARM]->name()  << endl;            
            output << endl;
            for (L_u8Idx=SHOULDER; L_u8Idx <= FOREARM; L_u8Idx++)
            {
                output << *ArmObj._Elt[L_u8Idx] ;
            }
            
            return output;            
        }        
    protected:
    private:
		std::string              _name;
        teArmState               _state;
        tsArmPosition            _pos;
        MotorSensorElementCl   * _Elt[3];

};


extern ArmCl * E_pArmObj;


#endif // ARM_H

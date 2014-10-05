/*!
 * \file hand.h
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
#ifndef __HAND_H__
#define __HAND_H__

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
 * \enum eHandState
 * \brief Hand state value
 *
 *  - HAND_RIGHT_CLOSE      : the element is undefined or close
 *  - HAND_RIGHT_READY      : the element is ready
 */
typedef enum eHandState {HAND_INIT=0, HAND_READY } teHandState;

typedef enum eHandType {HAND_LEFT=0, HAND_RIGHT, FINGER} teHandType;

typedef struct sHandPosition {
    int          rotation;
    int          updown;
    int gap;
} tsHandPosition;
    
 /**
 * \class HandCl
 * \brief 
 *
 */
class HandCl
{
    public:
		HandCl(iniCl *, std::string );
        void attach(teHandType, MotorSensorElementCl *);
        void unattach(teHandType);
        teHandState state();
        void enable();
        void disable();
        void sleep();
        void wakeup();
        void set(tsHandPosition);
        void get(tsHandPosition * );
        void rise(void);
        void fall(void);        
        bool calibrate();

        virtual ~HandCl();
        inline std::string name() { return _name; }
        unsigned int speed;
        unsigned int speed_mask;

        friend ostream &operator<<( ostream &output, 
                                    HandCl &HandObj )
        { 
            unsigned char L_u8Idx;
            output << "[" << HandObj._name << " ]" << endl;
            if (HandObj._Elt[HAND_LEFT] != NULL)   output << "hand left =" << HandObj._Elt[HAND_LEFT]->name()  << endl;
            if (HandObj._Elt[HAND_RIGHT] != NULL)  output << "hand right=" << HandObj._Elt[HAND_RIGHT]->name() << endl;
            if (HandObj._Elt[FINGER] != NULL)      output << "finger    =" << HandObj._Elt[FINGER]->name()     << endl;            
            output << endl;
            for (L_u8Idx=HAND_LEFT; L_u8Idx <= FINGER; L_u8Idx++)
            {
                output << *HandObj._Elt[L_u8Idx] ;
            }
            
            return output;            
        }        
    protected:
    private:
		std::string              _name;
        teHandState               _state;
        tsHandPosition            _pos;
        MotorSensorElementCl   * _Elt[3];
};


extern HandCl * E_pHandObj;


#endif // __HAND_RIGHT_H__

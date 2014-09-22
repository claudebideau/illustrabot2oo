/*!
 * \file Orientation.h
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
#ifndef ORIENTATION_H
#define ORIENTATION_H

/**   1. Standard Library Include                                   **/
#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <queue>
#include <pthread.h>


/**   1. Include files  (own)                                       **/
#include "arm.h"
#include "hand.h"
#include "inireader.h"

/**   2a.   External Functions                                      **/
/**   2b.   External Data                                           **/
/**   3.  Type Definitions                                          **/
/**   5.  Local Functions                                           **/
/**   6.  Macros / Defines                                          **/


typedef enum eOrientationState {ORIENTATION_INIT=0, ORIENTATION_CALIBRATE, ORIENTATION_READY, ORIENTATION_RUNNING,ORIENTATION_MAINTENANCE, ORIENTATION_STOPPED   } teOrientationState;


typedef struct sOrientation {
    tsArmPosition  arm;
    tsHandPosition hand;
} tsOrientation;

 /**
 * \class OrientationThCl
 * \brief 
 *
 */
class OrientationThCl
{
    public:
		OrientationThCl(iniCl * );

        teOrientationState state(void);

        static void *run(void *context)
        {
            return ((OrientationThCl *)context)->_execute();
        }

        void stop(void);
        std::string calibrateReq(void);
        std::string maintenance(std::string);
        int set(tsOrientation );
        void get(tsOrientation *);

        virtual ~OrientationThCl();


    protected:
    private:
        bool                       _bContinue;
        teOrientationState         _state;
        tsOrientation              _orientation;
        std::queue<tsOrientation>  _queue;
        RtTrace                  * _trace;
                                 
        ArmCl                    * _pArm;
        HandCl                   * _pHand;
        pthread_mutex_t            _mutex;

        void *_execute(void);        
        int   _calibrate(void);
};

extern const std::string ORIENTATION_KEYS[];
extern OrientationThCl * E_pOrientationThObj;


#endif // ORIENTATION_H

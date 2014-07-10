/*!
 * \file orientation.cpp
 * \author claude.bideau@gmail.com
 * \version 0.1
 * \brief This module provides a orientation declaration.
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
#include <cassert>
#include <stdexcept>
#include <iostream>
#include <unistd.h>
#include <map>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "gpio.h"
#include "orientation.h"
#include "trace.h"

using namespace std;


const std::string ORIENTATION_KEYS[] =  {"ORIENTATION_INIT", "ORIENTATION_CALIBRATE", "ORIENTATION_READY", "ORIENTATION_RUNNING", "ORIENTATION_STOPPED"};

OrientationThCl * E_pOrientationThObj= NULL;

OrientationThCl::OrientationThCl(iniCl * _pIni )
{
    std::string L_StrName;
    unsigned char L_u8Idx;
	std::string L_keys;
    std::list<std::string> F_listKey;
	std::list<std::string>::iterator L_it;

	
    _state     = ORIENTATION_INIT;
    _pArm   = NULL;
    _pWrist = NULL;
    _pHand  = NULL;

    TRACES_MSG("OrientationThCl: init");

    /* create and associate Rt Trace object */
    _trace = new RtTrace("OrientationThCl", RT_TRACE_ENABLE );
    _trace->trace(0, RT_TRACE_ENABLE, 0xFFFFFFFF, 2);
    
    // speed_mask = 0;
    // speed      = 0;
    _orientation.arm.longitude = 0;
    _orientation.arm.latitude  = 0;
    _orientation.arm.radius    = 0;

    /* TODO: need to check if exist section arm/wrist/hand */

    _pArm = new ArmCl(_pIni,"arm");
    E_pArmObj = _pArm;
    
    _state     = ORIENTATION_INIT;
    _bContinue = false;

	
}

/**
   @brief wakeup the stepper driver
   @return none.
 */
teOrientationState OrientationThCl::state(void)
{
    return _state;
}

int OrientationThCl::set(tsOrientation F_tsOrientation)
{ 
    _queue.push(F_tsOrientation);
    return 0;
}


void OrientationThCl::get(tsOrientation * F_ptsOrientation)
{ 
    F_ptsOrientation->arm.longitude = _orientation.arm.longitude;
    F_ptsOrientation->arm.latitude  = _orientation.arm.latitude ;
    F_ptsOrientation->arm.radius    = _orientation.arm.radius   ;
    return ;
}

void OrientationThCl::stop(void)
{ 
    _bContinue = false;
    return;
}


void * OrientationThCl::_calibrate(void)
{
    int L_i32Idx = 0;
    tsOrientation L_tsOrientationValue;
    
    TRACES_INFO("execute calibration of OrientationThCl" );
    _state = ORIENTATION_CALIBRATE;
    /* ================================================================= */
    /* execute code                                                      */
    /*   - calibrate hand                                                */
    /*   - calibrate wrist                                               */
    /*   - calibrate arm                                                 */
    /*                                                                   */
    /*  need to clarify how to calibrate depend of the current sensor    */
    /* ================================================================= */
    if (_pHand  != NULL) _pHand->start_calibrate();
    if (_pWrist != NULL) _pWrist->start_calibrate();
    if (_pArm   != NULL) _pArm->start_calibrate();
    
    _state = ORIENTATION_READY;
    TRACES_INFO("calibrate ending of OrientationThCl");
    return 0;
}

void *OrientationThCl::_execute(void)
{
    int L_i32Idx = 0;
    tsOrientation L_tsOrientationValue;
    _bContinue = true;
    
    TRACES_INFO("execute core of OrientationThCl" );
    _state = ORIENTATION_RUNNING;
    while (_bContinue==true)
    {
        /* ================================================ */
        /* execute code                                     */
        /* check input Queue                                */ 
        /*   - get new orientation                          */
        /*   - compute new orientation                      */
        /*   - set new orientation                          */
        /* rise arm                                         */
        /* rise wrist                                       */
        /* rise hand                                        */
        /* sleep before fall                                */
        /* fall arm                                         */
        /* fall wrist                                       */
        /* fall hand                                        */
        /*                                                  */
        /*                                                  */
        /* ================================================ */
        /* no orientation by default */
        memset(( void*) &L_tsOrientationValue, 0, sizeof(tsOrientation) );
        if (!_queue.empty())
        {
            /* - get new orientation          */
            /*   & suppress element in queue  */
            L_tsOrientationValue =  _queue.front();
            _queue.pop();
            /* - compute new orientation      */
            /* - set new orientation          */
        }

        if (_pArm   != NULL) _pArm->rise();
        if (_pWrist != NULL) _pWrist->rise();
        if (_pHand  != NULL) _pHand->rise();
        usleep(1000);
        if (_pArm   != NULL) _pArm->fall();
        if (_pWrist != NULL) _pWrist->fall();
        if (_pHand  != NULL) _pHand->fall();
        
    }
    _state = ORIENTATION_STOPPED;
    TRACES_INFO("end of core of OrientationThCl");
    return 0;
}


OrientationThCl::~OrientationThCl()
{
    _bContinue = false;
    if (_pArm    != NULL) delete _pArm   ;
    if (_pWrist  != NULL) delete _pWrist ;
    if (_pHand   != NULL) delete _pHand  ;
    _pArm = NULL;


    std::cout << "~OrientationThCl : delete done" << endl;
}


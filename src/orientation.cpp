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


const std::string ORIENTATION_KEYS[] =  {"ORIENTATION_INIT", "ORIENTATION_CALIBRATE", "ORIENTATION_READY", "ORIENTATION_RUNNING", "ORIENTATION_MAINTENANCE", "ORIENTATION_STOPPED"};

OrientationThCl * E_pOrientationThObj= NULL;

OrientationThCl::OrientationThCl(iniCl * _pIni )
{
    std::string L_StrName;
    unsigned char L_u8Idx;
    std::string L_keys;
    std::list<std::string> F_listKey;
    std::list<std::string>::iterator L_it;

    _pArm   = NULL;
    _pHand  = NULL;

    pthread_mutex_init(&_mutex,NULL);
    
    pthread_mutex_lock(&_mutex);
    _bContinue = false;
    _state     = ORIENTATION_INIT;
    pthread_mutex_unlock(&_mutex);
    
    TRACES_MSG("OrientationThCl: init");

    /* create and associate Rt Trace object */
    _trace = new RtTrace("OrientationThCl", RT_TRACE_ENABLE );
    _trace->trace(0, RT_TRACE_ENABLE, 0xFFFFFFFF, 2);
    
    // speed_mask = 0;
    // speed      = 0;
    _orientation.arm.longitude = 0;
    _orientation.arm.latitude  = 0;
    _orientation.arm.radius    = 0;
    _orientation.hand.rotation = 0;
    _orientation.hand.updown   = 0;
    _orientation.hand.gap      = 0;
    /* TODO: need to check if exist section arm/wrist/hand */

    _pArm  = new ArmCl(_pIni,"arm");
    _pHand = new HandCl(_pIni,"hand");
    E_pArmObj  = _pArm;
    E_pHandObj = _pHand;

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
    pthread_mutex_lock(&_mutex);
    _queue.push(F_tsOrientation);
    pthread_mutex_unlock(&_mutex);
    return 0;
}


void OrientationThCl::get(tsOrientation * F_ptsOrientation)
{ 
    F_ptsOrientation->arm.longitude = _orientation.arm.longitude;
    F_ptsOrientation->arm.latitude  = _orientation.arm.latitude ;
    F_ptsOrientation->arm.radius    = _orientation.arm.radius   ;
    F_ptsOrientation->hand.rotation = _orientation.hand.rotation;
    F_ptsOrientation->hand.updown   = _orientation.hand.updown  ;
    F_ptsOrientation->hand.gap      = _orientation.hand.gap     ;
    return ;
}

void OrientationThCl::stop(void)
{ 
    pthread_mutex_lock(&_mutex);
    _bContinue = false;
    pthread_mutex_unlock(&_mutex);
    return;
}

std::string OrientationThCl::calibrateReq(void)
{ 
    std::cout << " request Calibration in state : "<< _state <<endl;
    switch (_state)
    {
        case ORIENTATION_INIT:
        case ORIENTATION_STOPPED:
            break;
        case ORIENTATION_CALIBRATE:
        case ORIENTATION_MAINTENANCE:
        case ORIENTATION_READY:
        case ORIENTATION_RUNNING:
            pthread_mutex_lock(&_mutex);
            _state= ORIENTATION_CALIBRATE;
            pthread_mutex_unlock(&_mutex);
            break;
        default:
            break;
    }

    //~ std::cout << "change state to maintenance "<< F_state << " " << _state<< endl;
    if (_state == ORIENTATION_CALIBRATE)
        return std::string("on");
    return std::string("off");
}


std::string OrientationThCl::maintenance(std::string F_state="off")
{ 
    //~ std::cout << "change state to maintenance ? " << F_state << endl;
    if (F_state == "off")
    {
        switch (_state)
        {
            case ORIENTATION_INIT:
            case ORIENTATION_CALIBRATE:
            case ORIENTATION_READY:
            case ORIENTATION_STOPPED:
            case ORIENTATION_RUNNING:
                break;
            case ORIENTATION_MAINTENANCE:
                pthread_mutex_lock(&_mutex);
                _state= ORIENTATION_RUNNING;
                pthread_mutex_unlock(&_mutex);
                break;
            default:
                break;
        }
    }	else if (F_state == "on")
    {
        switch (_state)
        {
            case ORIENTATION_INIT:
            case ORIENTATION_CALIBRATE:
            case ORIENTATION_READY:
            case ORIENTATION_STOPPED:
            case ORIENTATION_MAINTENANCE:
                break;
            case ORIENTATION_RUNNING:
                pthread_mutex_lock(&_mutex);
                _state= ORIENTATION_MAINTENANCE;
                pthread_mutex_unlock(&_mutex);
                break;
                break;
            default:
                break;
        }
    }
    //~ std::cout << "change state to maintenance "<< F_state << " " << _state<< endl;
    if (_state == ORIENTATION_MAINTENANCE)
        return std::string("on");
    return std::string("off");
}


int OrientationThCl::_calibrate(void)
{
    int L_i32Idx = 0;
    bool L_bCal;
    bool L_bLoop;
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
    pthread_mutex_lock(&_mutex);
    L_bLoop = _bContinue;
    pthread_mutex_unlock(&_mutex);
    while (L_bLoop==true)
    {
        L_bCal = true;
        if (_pHand  != NULL) L_bCal &= _pHand->calibrate();
        if (_pArm   != NULL) L_bCal &= _pArm->calibrate();
        usleep(1000);
        if (_pArm   != NULL) _pArm->fall();
        if (_pHand  != NULL) _pHand->fall();
        
        /* continue ? Critical section */
        pthread_mutex_lock(&_mutex);
        L_bLoop = not (L_bCal) && _bContinue;
        pthread_mutex_unlock(&_mutex);
    }
    
    _state = ORIENTATION_READY;
    TRACES_INFO("calibrate ending of OrientationThCl");
    return 0;
}

void *OrientationThCl::_execute(void)
{
    int                 L_i32Idx        = 0;
    tsOrientation       L_tsOrientationValue;
    teOrientationState  L_state;

    _bContinue = true;
    
    TRACES_INFO("execute core of OrientationThCl" );


    /* first action is the calibration */
    _calibrate();
    
    
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
        /* rise hand                                        */
        /* sleep before fall                                */
        /* fall arm                                         */
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
            pthread_mutex_lock(&_mutex);
            L_tsOrientationValue =  _queue.front();
            _queue.pop();
            pthread_mutex_unlock(&_mutex);
            
            /* - compute new orientation      */
            /* - set new orientation          */
        }

        pthread_mutex_lock(&_mutex);
        L_state= _state;
        pthread_mutex_unlock(&_mutex);
        switch (L_state)
        {
            case ORIENTATION_RUNNING:
                if (_pArm   != NULL) _pArm->rise();
                if (_pHand  != NULL) _pHand->rise();
                usleep(1000);
                if (_pArm   != NULL) _pArm->fall();
                if (_pHand  != NULL) _pHand->fall();
                break;
            case ORIENTATION_CALIBRATE:
                _calibrate();
                break;
            default:
                /* no action :  maintenance case */
                usleep(1000);
                break;
        }

        
    }
    _state = ORIENTATION_STOPPED;
    TRACES_INFO("end of core of OrientationThCl");
    return 0;
}


OrientationThCl::~OrientationThCl()
{
    _bContinue = false;
    if (_pArm    != NULL) delete _pArm   ;
    if (_pHand   != NULL) delete _pHand  ;
    _pArm  = NULL;
    _pHand = NULL;


    std::cout << "~OrientationThCl : delete done" << endl;
}


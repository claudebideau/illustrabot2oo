/*!
 * \file arm.cpp
 * \author claude.bideau@gmail.com
 * \version 0.1
 * \brief This module provides a arm declaration.
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
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "gpio.h"
#include "arm.h"
#include "trace.h"

using namespace std;


const std::string ARM_KEYS[] =  {"shoulder", "arm", "forearm"};
ArmCl * E_pArmObj;

ArmCl::ArmCl(iniCl * _pIni, std::string F_ArmName)
{
    std::string L_StrName;
    unsigned char L_u8Idx;
	std::string L_keys;
    std::list<std::string> F_listKey;
	std::list<std::string>::iterator L_it;
	MotorSensorElementCl * L_pElt;
	
	_name      = F_ArmName;
    _state     = ARM_INIT;
    speed_mask = 0;
    speed      = 0;
    _pos.longitude = 0;
    _pos.latitude  = 0;
    _pos.radius    = 0;

    for (L_u8Idx=SHOULDER; L_u8Idx<= FOREARM ; L_u8Idx++)
    {
        _Elt[L_u8Idx] = NULL;
    }
	/* loop on all keys : 	step, dir, enable, m0, m1, m2, sleep */
	F_listKey = _pIni->getItems(_name);
    TRACES_MSG_ARG1("ArmCl: %s ini parsing",_name.c_str());

	if (!F_listKey.empty())
	{
		for (L_u8Idx=SHOULDER; L_u8Idx<= FOREARM ; L_u8Idx++)
		{
			L_StrName = _pIni->get(F_ArmName, ARM_KEYS[L_u8Idx] );
            // cout << "L_StrName = '" << L_StrName << "'" <<endl;
            // cout << "L_StrName.size() = " << L_StrName.size() << endl;
			if (L_StrName.size() >= 0)
			{
                // std::cout << "create Elt(" << L_StrName<<")"<< endl;
				L_pElt = new MotorSensorElementCl(_pIni, L_StrName);
                // std::cout << "attach Elt(" << L_StrName<<")"<< endl;

				attach((teArmType)L_u8Idx,L_pElt);
                // std::cout << "eo attach Elt(" << L_StrName<<")"<< endl;
                
			}
		}
	} else throw  std::string("undefined arm : ")+_name;
	
    _state     = ARM_READY;

	
}

 
void ArmCl::attach(teArmType F_ArmType, MotorSensorElementCl * F_pElt)
{
    
    if (_Elt[F_ArmType] != NULL) throw "already attached ( need to unattach before resetting)";
    _Elt[F_ArmType] = F_pElt;
    G_MapElementObj[ARM_KEYS[F_ArmType]] = F_pElt;

}

void ArmCl::unattach(teArmType F_ArmType)
{
    if (_Elt[F_ArmType] == NULL) throw "not attached";

    delete _Elt[F_ArmType];
    G_MapElementObj[ARM_KEYS[F_ArmType]] = NULL;
    _Elt[F_ArmType] =0;
    
}

void ArmCl::enable()
{
    unsigned char L_u8Idx;

    for (L_u8Idx=SHOULDER; L_u8Idx<= FOREARM ; L_u8Idx++)
        if (_Elt[L_u8Idx] != NULL)
            _Elt[L_u8Idx]->enable();
    return;
}
void ArmCl::disable()
{
    unsigned char L_u8Idx;

    for (L_u8Idx=SHOULDER; L_u8Idx<= FOREARM ; L_u8Idx++)
        if (_Elt[L_u8Idx] != NULL)
            _Elt[L_u8Idx]->disable();
    return;
}
void ArmCl::sleep()
{
    unsigned char L_u8Idx;

    for (L_u8Idx=SHOULDER; L_u8Idx<= FOREARM ; L_u8Idx++)
        if (_Elt[L_u8Idx] != NULL)
            _Elt[L_u8Idx]->sleep();
    return;

}
void ArmCl::wakeup()
{
    unsigned char L_u8Idx;

    for (L_u8Idx=SHOULDER; L_u8Idx<= FOREARM ; L_u8Idx++)
        if (_Elt[L_u8Idx] != NULL)
            _Elt[L_u8Idx]->wakeup();
    return;

}

/**
   @brief wakeup the stepper driver
   @return none.
 */
teArmState ArmCl::state(void)
{
    return _state;
}

void ArmCl::set(tsArmPosition F_tsArmPos)
{
    int L_i32alphaScaled, L_i32betaScaled;
    int L_i32AlphaAngle, L_i32GammaAngle;

    _pos.longitude = F_tsArmPos.longitude;
    _pos.latitude  = F_tsArmPos.latitude ;
    _pos.radius    = F_tsArmPos.radius   ;
    
    // input value are scaled
     //cout << "_pos.longitude"<< _pos.longitude << endl;
     //cout << "_pos.latitude "<< _pos.latitude  << endl;
     //cout << "_pos.radius   "<< _pos.radius    << endl;

    // determine longitude require step value
    _Elt[SHOULDER]->set(_pos.longitude);
    
    // processing depend of latitude and radius
    // both angle of the arm and forearm
    //   - determine radius impact
    //       radiusAngle = acos(_pos.radius)
    //L_i32AlphaAngle = G_tsaAlphaTable[_pos.radius].norm;

    _Elt[ARM]->set(_pos.latitude);
    _Elt[FOREARM]->set(_pos.radius);
    

 
#if 0
    // determine longitude require step value
    _Elt[SHOULDER]->set(_pos.longitude);
    
    // processing depend of latitude and radius
    // both angle of the arm and forearm
    //   - determine radius impact
    //       radiusAngle = acos(_pos.radius)
    L_i32AlphaAngle = G_tsaAlphaTable[_pos.radius].norm;

    _Elt[ARM]->set(_pos.latitude + L_i32AlphaAngle);

    L_i32GammaAngle = _pos.latitude - L_i32AlphaAngle;
    _Elt[FOREARM]->set(L_i32GammaAngle);
    cout << "_pos.latitude(scaled)   = " << _pos.latitude << endl;
    cout << "L_i32AlphaAngle(scaled) = " << L_i32AlphaAngle << endl;
    cout << "L_i32AlphaAngle(degre)  = " << G_tsaAlphaTable[_pos.radius].degre << endl;
    cout << "L_i32GammaAngle(scaled) = " << L_i32GammaAngle << endl;

#endif
        
    // determine longitude require step value
    
    return;
}

   


void ArmCl::get(tsArmPosition * F_ptsArmPos)
{
    F_ptsArmPos->longitude = _pos.longitude;
    F_ptsArmPos->latitude  = _pos.latitude ;
    F_ptsArmPos->radius    = _pos.radius   ;
    return;
}

/**
   @brief       step pulse
   @return none.
 */
void ArmCl::rise()
{
    unsigned char L_u8Idx;
    for (L_u8Idx=SHOULDER; L_u8Idx<= FOREARM ; L_u8Idx++)
    {
        if( _Elt[L_u8Idx] != NULL){
            _Elt[L_u8Idx]->rise();
        }
    }

}

/**
   @brief       clock fall
   @param[in]   value
   @return none.
 */
void ArmCl::fall()
{
    unsigned char L_u8Idx;
    for (L_u8Idx=SHOULDER; L_u8Idx<= FOREARM ; L_u8Idx++)
    {
        if( _Elt[L_u8Idx] != NULL)
            _Elt[L_u8Idx]->fall();
    }
}

bool ArmCl::calibrate(void)
{
    bool L_bCalib= true;
    uint8_t L_u8sensorArm =0xFF;
    uint8_t L_u8sensorForeArm = 0xFF;
    /* order of calibration        */
    /*   1) shoulder               */
    /*   2) arm //forearm          */
    /*   3) forearm //arm          */
    if( _Elt[SHOULDER] != NULL)
        L_bCalib &= _Elt[SHOULDER]->calibrate();

    if( _Elt[ARM] != NULL)      _Elt[ARM]->sensor(&L_u8sensorArm);
    if( _Elt[FOREARM] != NULL)  _Elt[FOREARM]->sensor(&L_u8sensorForeArm);

    std::cout << "arm = " << L_u8sensorArm << " / forearm = " << L_u8sensorForeArm << endl;

    if ((L_u8sensorArm == 0xFF )|| (L_u8sensorForeArm==0xFF)) return L_bCalib;

#if 0
    L_bCalib &= _Elt[ARM]->calibrate();
    L_bCalib &= _Elt[FOREARM]->calibrate();
#endif

    return L_bCalib;
}


ArmCl::~ArmCl()
{
    unsigned char L_u8Idx;
    for (L_u8Idx=SHOULDER; L_u8Idx<= FOREARM ; L_u8Idx++)
    {
        try {
            unattach((teArmType) L_u8Idx);
        } catch (const char* msg)
        {
            std::cout << msg <<endl;
        }
        catch (std::string msg)
        {
            std::cerr << msg << endl;
        }
    }
    std::cout << "~ArmCl : delete done" << endl;
}


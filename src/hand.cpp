/*!
 * \file hand.cpp
 * \author claude.bideau@gmail.com
 * \version 0.1
 * \brief This module provides a hand class
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

#include "gpio.h"
#include "hand.h"
#include "trace.h"

using namespace std;


const std::string HAND_KEYS[] =  {"left", "right", "finger"};
HandCl * E_pHandObj;

HandCl::HandCl(iniCl * _pIni, std::string F_HandName)
{
    std::string L_StrName;
    unsigned char L_u8Idx;
	std::string L_keys;
    std::list<std::string> F_listKey;
	std::list<std::string>::iterator L_it;
	MotorSensorElementCl * L_pElt;
	
	_name      = F_HandName;
    _state     = HAND_INIT;
    speed_mask = 0;
    speed      = 0;
    _pos.rotation = 0;
    _pos.updown  = 0;
    _pos.gap    = 0;

    for (L_u8Idx=HAND_LEFT; L_u8Idx<= FINGER ; L_u8Idx++)
    {
        _Elt[L_u8Idx] = NULL;
    }
	/* loop on all keys : 	step, dir, enable, m0, m1, m2, sleep */
	F_listKey = _pIni->getItems(_name);
    TRACES_MSG_ARG1("HandCl: %s ini parsing",_name.c_str());

	if (!F_listKey.empty())
	{
		for (L_u8Idx=HAND_LEFT; L_u8Idx<= FINGER ; L_u8Idx++)
		{
			L_StrName = _pIni->get(_name, HAND_KEYS[L_u8Idx] );
            cout << "L_StrName = '" << L_StrName << "'" <<endl;
            cout << "L_StrName.size() = " << L_StrName.size() << endl;
			if (L_StrName.size() >= 0)
			{
                std::cout << "create Elt(" << L_StrName<<")"<< endl;
				L_pElt = new MotorSensorElementCl(_pIni, L_StrName);
                std::cout << "attach Elt(" << L_StrName<<")"<< endl;

				attach((teHandType)L_u8Idx,L_pElt);
                std::cout << "eo attach Elt(" << L_StrName<<")"<< endl;
                
			}
		}
	} else throw  std::string("undefined hand_right : ")+_name;
	
    _state     = HAND_READY;

	
}

 
void HandCl::attach(teHandType F_HandType, MotorSensorElementCl * F_pElt)
{
    
    if (_Elt[F_HandType] != NULL) throw "already attached ( need to unattach before resetting)";
    _Elt[F_HandType] = F_pElt;
    G_MapElementObj[HAND_KEYS[F_HandType]] = F_pElt;

}

void HandCl::unattach(teHandType F_HandType)
{
    if (_Elt[F_HandType] == NULL) throw "not attached";

    delete _Elt[F_HandType];
    G_MapElementObj[HAND_KEYS[F_HandType]] = NULL;
    _Elt[F_HandType] =0;
    
}

void HandCl::enable()
{
    unsigned char L_u8Idx;

    for (L_u8Idx=HAND_LEFT; L_u8Idx<= FINGER ; L_u8Idx++)
        if (_Elt[L_u8Idx] != NULL)
            _Elt[L_u8Idx]->enable();
    return;
}
void HandCl::disable()
{
    unsigned char L_u8Idx;

    for (L_u8Idx=HAND_LEFT; L_u8Idx<= FINGER ; L_u8Idx++)
        if (_Elt[L_u8Idx] != NULL)
            _Elt[L_u8Idx]->disable();
    return;
}
void HandCl::sleep()
{
    unsigned char L_u8Idx;

    for (L_u8Idx=HAND_LEFT; L_u8Idx<= FINGER ; L_u8Idx++)
        if (_Elt[L_u8Idx] != NULL)
            _Elt[L_u8Idx]->sleep();
    return;

}
void HandCl::wakeup()
{
    unsigned char L_u8Idx;

    for (L_u8Idx=HAND_LEFT; L_u8Idx<= FINGER ; L_u8Idx++)
        if (_Elt[L_u8Idx] != NULL)
            _Elt[L_u8Idx]->wakeup();
    return;

}

/**
   @brief wakeup the stepper driver
   @return none.
 */
teHandState HandCl::state(void)
{
    return _state;
}

void HandCl::set(tsHandPosition F_tsHandPos)
{
    int L_i32alphaScaled, L_i32betaScaled;
    int L_i32AlphaAngle, L_i32GammaAngle;
    
    _pos.rotation = F_tsHandPos.rotation;
    _pos.updown   = F_tsHandPos.updown ;
    _pos.gap      = F_tsHandPos.gap   ;
    
    // input value are scaled
    // cout << "_pos.rotation"<< _pos.rotation << endl;
    // cout << "_pos.updown "<< _pos.updown  << endl;
    // cout << "_pos.gap   "<< _pos.gap    << endl;

    // determine rotation require step value
    //_Elt[HAND_LEFT]->set(_pos.rotation);
    _Elt[HAND_LEFT]->set(0);
    // processing depend of updown and gap
    // both angle of the hand_right and finger
    //   - determine gap impact
    //       gapAngle = acos(_pos.gap)
    L_i32AlphaAngle = G_tsaAlphaTable[_pos.gap].norm;

    //_Elt[HAND_RIGHT]->set(_pos.updown + L_i32AlphaAngle);
    _Elt[HAND_RIGHT]->set(0);
    L_i32GammaAngle = _pos.updown - L_i32AlphaAngle;
    //_Elt[FINGER]->set(L_i32GammaAngle);
    _Elt[FINGER]->set(0);
    // cout << "_pos.updown(scaled)   = " << _pos.updown << endl;
    // cout << "L_i32AlphaAngle(scaled) = " << L_i32AlphaAngle << endl;
    // cout << "L_i32AlphaAngle(degre)  = " << G_tsaAlphaTable[_pos.gap].degre << endl;
    // cout << "L_i32GammaAngle(scaled) = " << L_i32GammaAngle << endl;

        
        
    // determine rotation require step value
    
    return;
}

   


void HandCl::get(tsHandPosition * F_ptsHandPos)
{
    F_ptsHandPos->rotation = _pos.rotation;
    F_ptsHandPos->updown  = _pos.updown ;
    F_ptsHandPos->gap    = _pos.gap   ;
    return;
}

/**
   @brief       step pulse
   @return none.
 */
void HandCl::rise()
{
    unsigned char L_u8Idx;
    for (L_u8Idx=HAND_LEFT; L_u8Idx<= FINGER ; L_u8Idx++)
    {
        if( _Elt[L_u8Idx] != NULL)
            _Elt[L_u8Idx]->rise();
    }

}

/**
   @brief       clock fall
   @param[in]   value
   @return none.
 */
void HandCl::fall()
{
    unsigned char L_u8Idx;
    for (L_u8Idx=HAND_LEFT; L_u8Idx<= FINGER ; L_u8Idx++)
    {
        if( _Elt[L_u8Idx] != NULL)
            _Elt[L_u8Idx]->fall();
    }
}

bool HandCl::calibrate(void)
{
    cout <<  "WARNING: HAND CALIBRATION not yet implemented" << endl;
    return true;
}


HandCl::~HandCl()
{
    unsigned char L_u8Idx;
    for (L_u8Idx=HAND_LEFT; L_u8Idx<= FINGER ; L_u8Idx++)
    {
        try {
            unattach((teHandType) L_u8Idx);
        } catch (const char* msg)
        {
            std::cout << msg <<endl;
        }
        catch (std::string msg)
        {
            std::cerr << msg << endl;
        }
    }
    std::cout << "~HandCl : delete done" << endl;
}


/*!
 * \file easydrv.cpp
 * \author claude.bideau@gmail.com
 * \version 0.1
 * \brief This module provides a easydrv declaration.
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
#include <iostream>
#include "gpio.h"
#include "easydrv.h"
#include "trace.h"

const std::string GPIOKEYS[] =  {"step", "dir", "enable", "m0", "m1", "m2", "sleep"};


using namespace std;


EasyDriverCl::EasyDriverCl()
{
    unsigned char L_u8Idx;
    state = ED_INIT;
    currentDir = CLOCKWISE;
	_name = "";

    for (L_u8Idx=STEP; L_u8Idx<= SLEEP ; L_u8Idx++)
    {
        io[L_u8Idx] = NULL;
    }
}

EasyDriverCl::EasyDriverCl(iniCl * F_pini, std::string F_drvname)
{
    long L_s64GpioIdx;
    unsigned char L_u8Idx;
	std::string L_keys;
    std::list<std::string> F_listKey;
	std::list<std::string>::iterator L_it;
	GpioOutCl * L_pGpio;
	
	_name      = F_drvname;
    state      = ED_INIT;
    currentDir = CLOCKWISE;
    speed_mask = 0;
    speed      = 0;

    for (L_u8Idx=STEP; L_u8Idx<= SLEEP ; L_u8Idx++)
    {
        io[L_u8Idx] = NULL;
    }
	/* loop on all keys : 	step, dir, enable, m0, m1, m2, sleep */
	F_listKey = F_pini->getItems(_name);
    TRACES_MSG_ARG1("EasyDriverCl: %s ini parsing",_name.c_str());

	if (!F_listKey.empty())
	{
		for (L_u8Idx=STEP; L_u8Idx<= SLEEP ; L_u8Idx++)
		{
			L_s64GpioIdx = F_pini->getInteger(F_drvname,GPIOKEYS[L_u8Idx],-1);
			if (L_s64GpioIdx >= 0)
			{
                // std::cout << "create GPIO(" << L_s64GpioIdx<<")"<< endl;
				L_pGpio = new GpioOutCl((unsigned int) L_s64GpioIdx, LOW);
                // std::cout << "attach GPIO(" << L_s64GpioIdx<<")"<< endl;
				attach((eEdIOType)L_u8Idx,L_pGpio);
                // std::cout << "eo attach GPIO(" << L_s64GpioIdx<<")"<< endl;
			}
		}
	} else throw  std::string("undefined driver : ")+_name;
	

	
}

 
void EasyDriverCl::attach(teEdIOType F_iotype, GpioOutCl * F_io)
{
    unsigned int L_u32mask = 0x1;
    
    /* minimal require is DIR and STEP                               */
    /* others must be hard-wired set to correct value if not require */
    if (io[F_iotype] != NULL) throw "already attached ( need to unattach before resetting)";
    io[F_iotype] = F_io;
    
    switch(F_iotype)
    {
        case STEP:
            if (io[DIR] != NULL)
                state=ED_READY;
            io[F_iotype]->set(LOW);
            break;
        case DIR:
            if (io[STEP] != NULL)
                state=ED_READY;
            io[F_iotype]->set(currentDir);
            break;
        case ENABLE:
        case SLEEP:
            io[F_iotype]->set(HIGH);
            break;
        case M2: L_u32mask <<= 1;
        case M1: L_u32mask <<= 1;
        case M0: 
            speed_mask |= L_u32mask;
            io[F_iotype]->set(LOW);
            break;
        
    }
    io[F_iotype]->setDir();
}

void EasyDriverCl::unattach(teEdIOType F_iotype)
{
    /* minimal require is DIR and STEP                               */
    /* others must be hard-wired set to correct value if not require */
    //std::cout << "unattach IO[" << F_iotype << "] ";
    if (io[F_iotype] == NULL) throw "not attached";
    switch(F_iotype)
    {
        case STEP:
        case DIR:
            state=ED_INIT;
            break;
        case ENABLE:
        case M0: 
        case M1: 
        case M2: 
        case SLEEP:
            break;
    }
    delete io[F_iotype];
    //std::cout << " done" << endl;
}

void EasyDriverCl::enable()
{
    if (io[ENABLE] != NULL)
        io[ENABLE]->set(LOW);
}
void EasyDriverCl::disable()
{
    if (io[ENABLE] != NULL)
        io[ENABLE]->set(HIGH);
}
void EasyDriverCl::sleep()
{
    if (io[ENABLE] != NULL)
        io[ENABLE]->set(HIGH);
}
void EasyDriverCl::wakeup()
{
    if (io[ENABLE] != NULL)
        io[ENABLE]->set(LOW);
}

void EasyDriverCl::step_pulse(int F_i32Value)
{
    teValue L_teDir;
    teValue L_teStep;
    /* determine the motor direction */
    // std::cout << "step_pulse : "<<F_i32Value<<endl;
    L_teDir = (F_i32Value>=0) ? CLOCKWISE : ANTICLOCKWISE;
    L_teStep = (teValue) (F_i32Value&0x1);
    if (L_teDir != currentDir)
    {
        /* change motor direction */
        io[DIR]->set(L_teDir);
        currentDir= L_teDir;
    }
    io[STEP]->set(L_teStep);
    
}

teValue EasyDriverCl::dir()
{
    return currentDir;
}

void EasyDriverCl::step_fall()
{
    io[STEP]->set(LOW);
}

void EasyDriverCl::setSpeed(unsigned int F_u32Value)
{
    unsigned int L_u32Index;
    teValue  L_teValue;
    teEdIOType L_ioType;
    const teEdIOType L_aEdio[3] = {M0, M1, M2};
    
    /* M0 : bit 0 */
    /* M1 : bit 1 */
    /* M2 : bit 2 */
    // speed= F_u32Value;
    speed = 0;
    std::cout << "speed = " << F_u32Value << endl;
    for (L_u32Index=0; L_u32Index< 3; L_u32Index++)
    {
        L_ioType = L_aEdio[L_u32Index];
        L_teValue = (teValue) (F_u32Value & 0x1);
	std::cout <<  "L_u32Index = " << L_u32Index << " / L_ioType=" << L_ioType << endl;
        if (io[L_ioType] != NULL)
        {
            std::cout << "setSpeed / IO speed = (" <<L_ioType << ") value = " <<L_teValue  << endl;
        
            io[L_ioType]->set(L_teValue);
            speed |= (1<<L_u32Index);
        } else {
            std::cout << "setSpeed / IO speed (" <<L_ioType << ") not connected !!! "<< endl;
        
        }
        F_u32Value = F_u32Value>>1;
    }

    return;
}
 

EasyDriverCl::~EasyDriverCl()
{
    unsigned char L_u8Idx;
    for (L_u8Idx=STEP; L_u8Idx<= SLEEP ; L_u8Idx++)
    {
        try {
            unattach((teEdIOType) L_u8Idx);
        } catch (const char* msg)
        {
            std::cout << msg <<endl;
        }
        catch (std::string msg)
        {
            std::cerr << msg << endl;
        }
    }
    std::cout << "~EasyDriverCl : delete done" << endl;
}


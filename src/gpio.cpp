/*!
 * \file gpio.hpp
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
#include <sstream> 
#include <fstream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include "gpio.h"

using namespace std;

static inline int gpio_export(unsigned int gpio)
{
    ofstream L_fd;
    
#ifdef __TEST
    int status;
    char cmd[1024];
    system("mkdir -p " SYSFS_GPIO_DIR);
    snprintf(cmd,1024, "mkdir -p " SYSFS_GPIO_DIR "/gpio%d",gpio);
    system(cmd);
    snprintf(cmd,1024, "echo 1 > " SYSFS_GPIO_DIR "/gpio%d/value",gpio);
    system(cmd);
    snprintf(cmd,1024, "echo 'in' > " SYSFS_GPIO_DIR "/gpio%d/value",gpio);
    system(cmd);
   
#endif
    L_fd.open (SYSFS_GPIO_DIR "/export");
    if (! L_fd.is_open())
    {
       throw std::string("Error: impossible to open " SYSFS_GPIO_DIR "/export");
    }    
    L_fd << gpio;
    L_fd.close();

    return 0;
}

static inline int gpio_unexport(unsigned int gpio)
{
    ofstream L_fd;
    
    L_fd.open (SYSFS_GPIO_DIR "/unexport");
    if (! L_fd.is_open())
    {
       throw std::string("Error: impossible to open " SYSFS_GPIO_DIR "/unexport");
    }    
    L_fd << gpio;
	L_fd.close();
	return 0;
}



GpioCl::GpioCl(unsigned int F_u32id, teDirection F_teDir, teValue F_teValue)
{
    state = CLOSE;

    /* set main parameters */
    id = F_u32id;

    snprintf(iodirname, MAX_BUF, SYSFS_GPIO_DIR  "/gpio%d/direction", F_u32id);
    snprintf(iovaluename, MAX_BUF, SYSFS_GPIO_DIR  "/gpio%d/value", F_u32id);

    // cout << "io = " << F_u32id << " dir=" << F_teDir << " value=" << F_teValue << " iodirname=" << iodirname << " iovaluename="<< iovaluename<< endl;
    
    if (gpio_export(F_u32id) != 0) throw "GpioCl : Export impossible";
    if (setDir() != 0) throw  "GpioCl : SetDir impossible";

    dir   = F_teDir;
    F_teValue = get();
    state = OPEN;
    // cout << "io = " << F_u32id << " dir=" << F_teDir << " value=" << F_teValue << " iodirname=" << iodirname << " iovaluename="<< iovaluename << " done" << endl;
}

GpioCl::~GpioCl()
{
    dir   = INPUT_PIN;

    if (setDir() != 0) throw "~GpioCl : SetDir impossible";
    if (gpio_unexport(id) != 0)  throw "~GpioCl : unexport impossible";
    // cout << "io[" << id << "] delete" << endl;
    state = CLOSE;
}


/**
 * \fn get
 * \brief
 * \author Claude Bideau
 * \date  2014/03/21
 * \return 0
 * \details
 * \note
 * \todo
 *
 */
teValue GpioCl::get()
{
    ifstream L_fd;
	int L_s32Value = 0;
    string   line;
    char * L_pcStr;

    L_fd.open(iovaluename);
    
    if (L_fd.is_open())
    {
        getline (L_fd,line);
        L_pcStr = new char [line.length()+1];
        std::strcpy (L_pcStr, line.c_str());

        L_s32Value = atoi(L_pcStr);

    } else {
            throw std::string("Error: impossible to open ")+iovaluename;        
    }
    L_fd.close();
    if ((L_s32Value != LOW) && (L_s32Value != HIGH)) throw std::string("convertion issue on ")+iovaluename+"value ="+L_pcStr;
    
	return (teValue)L_s32Value;
}

/**
 * \fn GpioCl::set
 * \brief
 * \author Claude Bideau
 * \date  2014/03/21
 * \return 0
 * \details
 * \note
 * \todo
 *
 */
int GpioCl::set(teValue F_teValue)
{
    ofstream L_fd;
	const char L_s8aValue[2][2] = { "0", "1" };
    // cout << "set(" << id << ") iovaluename = "<< iovaluename <<endl;

	L_fd.open(iovaluename);
    if (! L_fd.is_open())
    {
           throw std::string("Error: impossible to open ")+iovaluename;
    }
    // cout << "write " << L_s8aValue[F_teValue] << " in gpio(" << id<<")" <<endl;
    L_fd << L_s8aValue[F_teValue];
    L_fd.close();

	return 0;
}


/**
 * \fn GpioCl::setDir
 * \brief Set Direction IO handler
 * \author Claude Bideau
 * \date  2014/03/21
 * \return 0
 * \details
 * \note
 * \todo
 *
 */
int GpioCl::setDir()
{
    ofstream L_fd;

	L_fd.open(iodirname);
	if (! L_fd.is_open())
	{
		   throw std::string("Error: impossible to open ")+iodirname;;
	}

	if (dir == OUTPUT_PIN)
		L_fd << "out";
	else
		L_fd << "in";
	L_fd.close();
	return 0;
}

/**
 * \fn GpioInCl::get
 * \brief
 * \author Claude Bideau
 * \date  2014/03/21
 * \return 0
 * \details
 * \note
 * \todo
 *
 */
teValue GpioInCl::get(void) 
{ 
    return GpioCl::get();
}

/**
 * \fn GpioOutCl::get
 * \brief
 * \author Claude Bideau
 * \date  2014/03/21
 * \return 0
 * \details
 * \note
 * \todo
 *
 */
teValue GpioOutCl::get(void) 
{ 
    return GpioCl::get();
}

/**
 * \fn GpioOutCl::set
 * \brief
 * \author Claude Bideau
 * \date  2014/03/21
 * \return 0
 * \details
 * \note
 * \todo
 *
 */
 int GpioOutCl::set(teValue F_teValue) 
{ 
    return GpioCl::set(F_teValue);
}

/*!
 * \file gpio.h
 * \author claude.bideau@gmail.com
 * \version 0.1
 * \brief This module provides a gpio declaration.
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
 #ifndef GPIO_H
#define GPIO_H
/**   1. Standard Library Include                                   **/
#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>

/**   1. Include files  (own)                                       **/
/**   2a.   External Functions                                      **/
/**   2b.   External Data                                           **/
/**   3.  Type Definitions                                          **/
/**   5.  Local Functions                                           **/
/**   6.  Macros / Defines                                          **/
#ifdef __TEST
#define SYSFS_GPIO_DIR  "/tmp/gpio"
#else
#define SYSFS_GPIO_DIR  "/sys/class/gpio"
#endif
#define MAX_BUF 64
#define SYSFS_OMAP_MUX_DIR "/sys/kernel/debug/omap_mux/"

/**
 * \enum eIOState
 * \brief IO state value
 *
 *  - close      : the element is undefined or close
 *  - open       : the element is initialize and open
 */
typedef enum eIOState {CLOSE=0, OPEN} teIOState;

/**
 * \enum eDirection
 * \brief Element state value
 *
 *  - INPUT_PIN  :
 *  - OUTPUT_PIN :
 *
 */
typedef enum eDirection{ INPUT_PIN=0, OUTPUT_PIN=1} teDirection;

 /**
 * \enum eValue
 * \brief Element state value
 *
 *  - LOW  :
 *  - HIGH :
 *
 */
typedef enum eValue{	LOW=0,	HIGH=1 } teValue;

 /**
 * \class GpioCl
 * \brief 
 *
 */
class GpioCl
{
    public:
        GpioCl(unsigned int F_u32id, teDirection F_teDir=INPUT_PIN, teValue F_teValue=LOW);
        virtual int set(teValue);
        virtual teValue get();
        virtual int setDir();
        virtual ~GpioCl();
        unsigned int id;
    protected:
    private:
        teIOState    state;
        teDirection  dir;
        teValue      value;
        char         iodirname[MAX_BUF];
        char         iovaluename[MAX_BUF];

};

 /**
 * \class GpioInCl
 * \brief 
 *
 */
class GpioInCl : public GpioCl
{
    public:
        GpioInCl(unsigned int F_u32id) : GpioCl(F_u32id,INPUT_PIN,LOW) {};
        teValue get();

};

 /**
 * \class GpioOutCl
 * \brief 
 *
 */
class GpioOutCl : public GpioCl
{
    public:
        GpioOutCl(unsigned int F_u32id, teValue F_teValue) : GpioCl(F_u32id,OUTPUT_PIN,F_teValue)  {};
        int set(teValue);
        teValue get();

};

#endif // GPIO_H

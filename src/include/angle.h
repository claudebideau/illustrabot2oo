/*!
 * \file angle.h
 * \author claude.bideau@gmail.com
 * \version 0.1
 * \brief 
 * \date  2014/05/01
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
#ifndef __ANGLE_H
#define __ANGLE_H

/**   1. Standard Library Include                                   **/

/**   1. Include files  (own)                                       **/

/**   2a.   External Functions                                      **/
/**   2b.   External Data                                           **/
/**   3.  Type Definitions                                          **/

typedef struct {
    int degre;
    int norm;
} tsAlphaAngle;

typedef struct {
    int degre;
    int norm;
} tsMinLatitudeAngle;

/**   5.  Local Functions                                           **/
/**   6.  Macros / Defines                                          **/

#define PI 3.14159265
#define QUANTIFICATION_LENGTH 15
#define MAX_SIZE ( 1<<(QUANTIFICATION_LENGTH))

#define PI_SCALED_POWER        15
#define PI_SCALED_VALUE       (1<<PI_SCALED_POWER)
#define HALF_PI_SCALED_VALUE  (1<<(PI_SCALED_POWER-1))

extern tsAlphaAngle G_tsaAlphaTable[MAX_SIZE+1];
extern tsMinLatitudeAngle G_tsaMinLatitudeTable[MAX_SIZE+1];

extern void Q_vInitAlphaTable(void);
extern void Q_vInitCheckLattitudeTable(short F_i16Heigh);


#endif // __ANGLE_H

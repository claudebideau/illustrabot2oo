/*!
 * \file angle.cpp
 * \author claude.bideau@gmail.com
 * \version 0.1
 * \brief 
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
 /**   1. Standard Library Include                                   **/
#include <cmath>
#ifdef __TEST
#include <iostream>
#endif
/**   1. Include files  (own)                                       **/
#include "trace.h"

using namespace std;

#define PI 3.14159265
#define QUANTIFICATION_LENGTH 12
#define MAX_SIZE ( 1<<QUANTIFICATION_LENGTH)



typedef struct {
    short degre;
    short norm;
} tsAlphaAngle;

tsAlphaAngle G_tsaAlphaTable[MAX_SIZE+1];

static inline short Q_i16Alpha( unsigned short F_u16Distance, unsigned short F_u16minDist)
{
    /* security */
    if (F_u16Distance > MAX_SIZE) F_u16Distance = MAX_SIZE;
    if (F_u16Distance < F_u16minDist) F_u16Distance = F_u16minDist;
    
    return G_tsaAlphaTable[F_u16Distance].norm;
}


void Q_vInitAlphaTable(void)
{
    unsigned short L_u16Index;
    double L_dIndex;
    double L_dValue;
    long int L_i64Value;
    for (L_u16Index=0; L_u16Index <= MAX_SIZE; L_u16Index++)
    {
        L_dIndex = (double)L_u16Index;
        L_dValue = acos(L_dIndex/MAX_SIZE);
        L_i64Value = lrint(L_dValue*MAX_SIZE/PI);
        G_tsaAlphaTable[L_u16Index].degre = (short)L_i64Value;
        L_i64Value = lrint(L_dValue*180/PI);
        G_tsaAlphaTable[L_u16Index].norm  = (short)L_i64Value;
#ifdef __TEST
        std::cout << L_u16Index  << " / " << L_dValue<<  " / " << L_i64Value << " / " << G_tsaAlphaTable[L_u16Index].degre << " / " << G_tsaAlphaTable[L_u16Index].norm << endl;
#endif
    }
    return;
}


// determine Alpha Angle
// r/2a is named D(distance)
// example of quantification:
// x < D <=2^12 ( x must be determine / example 0.25 * 2^12 = 1024 )

// A=cos_inv(D)
// cos_inv could be a table of 2^12 ( 4096 value of short)
// determine Theta Angle
// T = A + lat T norm = (A norm + lat norm) >>1
// determine Gamma Angle
// G = lat - A G norm = (lat norm - A norm ) >>1 

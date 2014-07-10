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
#include <iostream>
#include <fstream>
/**   1. Include files  (own)                                       **/
#include "trace.h"
#include "angle.h"

using namespace std;



tsAlphaAngle G_tsaAlphaTable[MAX_SIZE+1];
tsMinLatitudeAngle G_tsaMinLatitudeTable[MAX_SIZE+1];

static inline short Q_i16Alpha( unsigned short F_u16Distance, unsigned short F_u16minDist)
{
    /* security */
    if (F_u16Distance > MAX_SIZE) F_u16Distance = MAX_SIZE;
    if (F_u16Distance < F_u16minDist) F_u16Distance = F_u16minDist;
    
    return G_tsaAlphaTable[F_u16Distance].norm;
}

/**
 * \fn Q_vInitAlphaTable
 * \brief
 * \author Claude Bideau
 * \date  2014/05/21
 * \return 0
 * \details
 * \note
 * \todo
 *
 */
void Q_vInitAlphaTable(void)
{
    unsigned int L_u32Index;
    double L_dIndex;
    double L_dValue;
    long int L_i64Value;
    ofstream L_pAlphaFile;
    L_pAlphaFile.open("alpha.txt");
    L_pAlphaFile << "MAX_SIZE=" << MAX_SIZE << endl;
    L_pAlphaFile << "[index]\tacos\t64 bits\tdegre\tscaled" << endl;
    
    for (L_u32Index=0; L_u32Index <= MAX_SIZE; L_u32Index++)
    {
        L_dIndex = (double)L_u32Index;
        L_dValue = acos(L_dIndex/MAX_SIZE);
        L_i64Value = lrint(L_dValue*2*MAX_SIZE/PI);
        G_tsaAlphaTable[L_u32Index].norm = (int)L_i64Value;
        L_i64Value = lrint(L_dValue*2*90.0/PI);
        G_tsaAlphaTable[L_u32Index].degre  = (int)L_i64Value;
        L_pAlphaFile << "[" << L_u32Index  << "]\t" << L_dValue<<  "\t" << L_i64Value << "\t" << G_tsaAlphaTable[L_u32Index].degre << "\t" << G_tsaAlphaTable[L_u32Index].norm << endl;

    }
    return;
}

/**
 * \fn Q_vInitCheckLattitudeTable
 * \brief
 * \author Claude Bideau
 * \date  2014/05/21
 * \return 0
 * \details
 * \note
 * \todo
 *
 */
void Q_vInitCheckLattitudeTable(short F_i16Heigh)
{
    unsigned int L_u32RadiusIndex;
    double L_dIndex;
    double L_dValue;
    long int L_i64Value;
    // for each radius length
    // determine the acos
    for (L_u32RadiusIndex=0; L_u32RadiusIndex <= MAX_SIZE; L_u32RadiusIndex++)
    {
        if (L_u32RadiusIndex >= F_i16Heigh)
        {
            L_dIndex = (double)F_i16Heigh / (double)L_u32RadiusIndex;
            L_dValue = acos(L_dIndex);
            L_i64Value = lrint(L_dValue*2*MAX_SIZE/PI);
            G_tsaMinLatitudeTable[L_u32RadiusIndex].norm = (int)L_i64Value;
            L_i64Value = lrint(L_dValue*2*90.0/PI);
            G_tsaMinLatitudeTable[L_u32RadiusIndex].degre  = (int)L_i64Value;
#ifdef __TEST
            // std::cout << L_u32RadiusIndex  << " / " << L_dIndex<<  " / " << L_dValue <<  " / " ;
            // std::cout << L_i64Value << " / " << G_tsaMinLatitudeTable[L_u32RadiusIndex].degre << " / " << G_tsaMinLatitudeTable[L_u32RadiusIndex].norm << endl;
#endif
        } else {
            G_tsaMinLatitudeTable[L_u32RadiusIndex].degre  = 0;
            G_tsaMinLatitudeTable[L_u32RadiusIndex].norm   = 0;
        }
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

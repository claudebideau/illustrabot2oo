/*!
 * \file rttrace.cpp
 * \author claude.bideau@gmail.com
 * \version 0.1
 * \brief This module provides a calibrate declaration.
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

#include <cassert>
#include <stdexcept>
#include <iostream>
#include <unistd.h>
#include "trace.h"
#include "rttrace.h"

/**   1. Include files  (own)                                       **/

using namespace std;



RtTraceMap G_RtTraceMapObj;

RtTrace::RtTrace(std::string F_name, unsigned short F_level )
{
    _name      = F_name;
    TRACES_MSG_ARG1("RtTrace: %s",F_name.c_str());
    
    _u16level  = F_level;
    _u32size   = MAX_BUFFER_SIZE;
    _u32cindex = 0;
    _u32mask   = MAX_BUFFER_SIZE-1;
    memset ( _u32aBuffer, 0, sizeof(unsigned int)*MAX_BUFFER_SIZE);
    // cout << "Create RtTrace for '" << _name << "' module"<<endl;
    G_RtTraceMapObj.add(F_name, this);
    format = "TS|TnS|CF|P1|P2|P3|P4|P5";
    
}

std::list<std::string> RtTraceMap::list(void)
{
    std::list<std::string> L_list;
    std::map<std::string, RtTrace *>::const_iterator it;

    for (it = _rttrace.begin(); it != _rttrace.end(); ++it)
    {
        L_list.push_back(it->first);
    }
    return L_list;
}


void RtTraceMap::add(std::string F_name, RtTrace * F_rttrace)
{
    _rttrace[F_name] = F_rttrace;
}


RtTrace * RtTraceMap::get(std::string F_name)
{
    std::map<std::string,RtTrace *>::iterator it;
    it=_rttrace.find(F_name);
    if (it !=  _rttrace.end())
        return _rttrace[F_name];
    return NULL;
}

void RtTraceMap::erase(std::string F_name)
{
    std::map<std::string,RtTrace *>::iterator it;
    it=_rttrace.find(F_name);
    if (it !=  _rttrace.end())
        _rttrace.erase (it);
    return;
}


RtTraceMap::~RtTraceMap(void)
{
}

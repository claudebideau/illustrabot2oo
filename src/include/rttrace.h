#ifndef RTTRACE_H
#define RTTRACE_H
/*!
 * \file rttrace.h
 * \author claude.bideau@gmail.com
 * \version 0.1
 * \brief This module provides the real trace mechanism
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
#include <time.h>
#include <iostream>
#include <map>
#include <list>
#include <cstring>


/**   1. Include files  (own)                                       **/

using namespace std;

#define MAX_BUFFER_SIZE         (1024)
#define RT_TRACE_ENABLE         (0x00000001)



 /**
 * \class RtTrace
 * \brief 
 *
 */
class RtTrace
{
    public:

        RtTrace(std::string, unsigned short );
        ~RtTrace();
        unsigned short getLevel();
        void setLevel(unsigned short );
        unsigned int getTrace(unsigned int * );
        void trace(unsigned short , unsigned short, unsigned int, unsigned int);
        void trace(unsigned short , unsigned short, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
        std::string format;
        
    private:
        std::string  _name;
        unsigned short _u16level;
        unsigned int   _u32size;
        unsigned int   _u32cindex;
        unsigned int   _u32mask;
        unsigned int   _u32aBuffer[MAX_BUFFER_SIZE];
        struct timespec _time;
};


class RtTraceMap
{
    public:
        RtTraceMap();
        ~RtTraceMap();
        void add(std::string,RtTrace *);
        void erase(std::string);
        std::list<std::string> list();
        RtTrace * get(std::string);
    private:
        std::map<std::string,RtTrace *> _rttrace;
};

extern RtTraceMap G_RtTraceMapObj;

inline void RtTrace::trace(unsigned short F_cmd, unsigned short F_critere, unsigned int F_param1, unsigned int F_param2)
{
    if (( (_u16level & F_critere) != 0)&&(_u16level & RT_TRACE_ENABLE))
    {
        // cout << _u16level << " " << F_critere << endl;
        clock_gettime(CLOCK_REALTIME,&_time);
        _u32aBuffer[_u32cindex++] = _time.tv_sec;
        _u32aBuffer[_u32cindex++] = _time.tv_nsec;        
        _u32aBuffer[_u32cindex++] = (F_cmd << 16) & F_critere& 0xFFFF;
        _u32aBuffer[_u32cindex++] = F_param1;
        _u32aBuffer[_u32cindex++] = F_param2;
        _u32aBuffer[_u32cindex++] = 0;
        _u32aBuffer[_u32cindex++] = 0;
        _u32aBuffer[_u32cindex++] = 0;
        _u32cindex = _u32cindex & _u32mask;
        _u32aBuffer[_u32cindex]   = 0xFFFFFFFF;
    }
}


inline void RtTrace::trace(unsigned short F_cmd, unsigned short F_critere, unsigned int F_param1, unsigned int F_param2, unsigned int F_param3, unsigned int F_param4, unsigned int F_param5)
{
    if (( (_u16level & F_critere) != 0)&&(_u16level & RT_TRACE_ENABLE))
    {
        // cout << _u16level << " " << F_critere << endl;
        clock_gettime(CLOCK_REALTIME,&_time);
        _u32aBuffer[_u32cindex++] = _time.tv_sec;
        _u32aBuffer[_u32cindex++] = _time.tv_nsec;        
        _u32aBuffer[_u32cindex++] = (F_cmd << 16) & F_critere& 0xFFFF;
        _u32aBuffer[_u32cindex++] = F_param1;
        _u32aBuffer[_u32cindex++] = F_param2;
        _u32aBuffer[_u32cindex++] = F_param3;
        _u32aBuffer[_u32cindex++] = F_param4;
        _u32aBuffer[_u32cindex++] = F_param5;
        _u32cindex = _u32cindex & _u32mask;
        _u32aBuffer[_u32cindex]   = 0xFFFFFFFF;
    }
}


inline unsigned short RtTrace::getLevel()
{
    return _u16level;
}

inline void RtTrace::setLevel(unsigned short F_u16level)
{
    _u16level = F_u16level;
}

inline unsigned int RtTrace::getTrace(unsigned int * F_pu32Buffer)
{
    memcpy(F_pu32Buffer,_u32aBuffer, sizeof(unsigned int) * MAX_BUFFER_SIZE);
    return _u32cindex & ((MAX_BUFFER_SIZE-1)&(~7));
}

inline RtTraceMap::RtTraceMap(void)
{
}

#endif /* RTTRACE_H */

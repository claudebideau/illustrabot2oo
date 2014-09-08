/*!
 * \file RobotClientTh.h
 * \author claude.bideau@gmail.com
 * \version 0.1
 * \brief This module provides a clientTh declaration.
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
#ifndef __ROBOT_CLIENT_TH_H__
#define __ROBOT_CLIENT_TH_H__

/**   1. Standard Library Include                                   **/
#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <queue>
#include <pthread.h>

/**   1. Include files  (own)                                       **/
#include "trace.h"
#include "rttrace.h"
#include "tcpconnector.h"
#include "proto.h"
#include "orientation.h"

/**   2a.   External Functions                                      **/
/**   2b.   External Data                                           **/
/**   3.  Type Definitions                                          **/
/**   5.  Local Functions                                           **/
/**   6.  Macros / Defines                                          **/

#define MAX_RETRY 5



typedef enum eRCThreadState {RC_INIT=0, RC_CONNECTED,  RC_RUNNING, RC_CALIBRATE_REQ, RC_STOPPED } teRCThreadState;

 /**
 * \class RobotClientThreadCl
 * \brief 
 *
 */
class RobotClientThreadCl
{
    public:
		RobotClientThreadCl(std::string, int);

        teRCThreadState state(void);

        static void *run(void *context)
        {
            return ((RobotClientThreadCl *)context)->_execute();
        }

        void stop(void);

        virtual ~RobotClientThreadCl();


    protected:
    private:
        bool               _bContinue;
        bool               _bStop;
        teRCThreadState    _state;
        RtTrace          * _trace;
        TCPStream        * _stream;
        TCPConnector     * _connector;
        
        std::string        _host;
        int                _port;
        tsSocketStat       _stat;
        tsMsgRobotSrv      _tsMsgTx;
        tsMsgRobotSrv      _tsMsgRx;

        pthread_mutex_t    _mutex;
        // std::string __getHost(void);
        // int __getPort(void);

        void *_execute(void);        
};

extern const std::string ROBOT_CLIENT_KEYS[];



#endif // __ROBOT_CLIENT_TH_H__

/*!
 * \file RobotSrvTh.h
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
#ifndef __ROBOT_TH_H__
#define __ROBOT_TH_H__

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
#include "tcpacceptor.h"
#include "proto.h"

/**   2a.   External Functions                                      **/
/**   2b.   External Data                                           **/
/**   3.  Type Definitions                                          **/
/**   5.  Local Functions                                           **/
/**   6.  Macros / Defines                                          **/

#define MAX_TX_FIFO_SIZE (10)
#define MAX_RETRY 5

typedef enum eRobotSrvThState {ROBOT_TH_INIT=0, ROBOT_TH_CONNECTED, ROBOT_TH_RUNNING, ROBOT_TH_STOPPED   } teRobotSrvThState;

 /**
 * \class RobotSrvThread
 * \brief 
 *
 */
class RobotSrvThreadCl
{
    public:
		RobotSrvThreadCl(int);

        teRobotSrvThState state(void);

        static void *run(void *context)
        {
            return ((RobotSrvThreadCl *)context)->_execute();
        }

        // bool addMsg(char*,uint32_t);
        bool addMsg(tsMsgRobotSrv * );
        bool addData(tsUePayload *);
        void stop(void);

        virtual ~RobotSrvThreadCl();


    protected:
    private:
        bool               _bContinue;
        bool               _bStop;
        teRobotSrvThState     _state;
        RtTrace          * _trace;
        TCPStream        * _stream;
        TCPAcceptor      * _acceptor;
        pthread_mutex_t    _mutexTh;
        pthread_mutex_t    _mutexFifo;

        int                _port;
        tsSocketStat       _stat;
        tsMsgRobotSrv      _tsMsgTx;
        tsMsgRobotSrv      _tsMsgRx;

        std::queue<tsMsgRobotSrv *> _TxFifo;

        // std::string __getHost(void);
        // int __getPort(void);

        void *_execute(void);
        
        bool _mng_rx(void);
        bool _mng_tx(void);
};

extern const std::string ROBOT_TH_KEYS[];



#endif // __ROBOT_TH_H__

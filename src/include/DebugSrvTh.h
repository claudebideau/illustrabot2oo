/*!
 * \file DebugSrvTh.h
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
#ifndef __DEBUG_SRV_TH_H__
#define __DEBUG_SRV_TH_H__

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
#include "RobotSrvTh.h"
#include "UeSrvTh.h"

/**   2a.   External Functions                                      **/
/**   2b.   External Data                                           **/
/**   3.  Type Definitions                                          **/
/**   5.  Local Functions                                           **/
/**   6.  Macros / Defines                                          **/

#define MAX_TX_FIFO_SIZE (10)
#define MAX_RETRY 5

typedef enum eDebugSrvThState {DEBUG_SRV_TH_INIT=0, DEBUG_SRV_TH_CONNECTED, DEBUG_SRV_TH_RUNNING, DEBUG_SRV_TH_STOPPED   } teDebugSrvThState;

 /**
 * \class DebugSrvThread
 * \brief 
 *
 */
class DebugSrvThreadCl
{
    public:
		DebugSrvThreadCl(int, RobotSrvThreadCl *, UeSrvThreadCl *);

        teDebugSrvThState state(void);

        static void *run(void *context)
        {
            return ((DebugSrvThreadCl *)context)->_execute();
        }

        // bool addMsg(char*,uint32_t);
        bool addMsg(tsMsgDebugSrv * );
        // bool addData(tsUePayload *);
        void stop(void);

        virtual ~DebugSrvThreadCl();


    protected:
    private:
        bool                        _bContinue;
        bool                        _bStop;
        teDebugSrvThState              _state;
        RtTrace                   * _trace;
        TCPStream                 * _stream;
        TCPAcceptor               * _acceptor;
        pthread_mutex_t             _mutexTh;
        pthread_mutex_t             _mutexFifo;
                                  
        int                         _port;
        tsSocketStat                _stat;
        tsMsgDebugSrv               _tsMsgTx;
        tsMsgDebugSrv               _tsMsgRx;
        RobotSrvThreadCl          * _pts2RobotTh;
        UeSrvThreadCl             * _pts2UeTh;
        std::queue<tsMsgDebugSrv *> _TxFifo;

        // std::string __getHost(void);
        // int __getPort(void);

        void *_execute(void);
        
        bool _mng_rx(void);
        bool _mng_tx(void);
};

extern const std::string DEBUG_SRV_TH_KEYS[];



#endif // __DEBUG_SRV_TH_H__

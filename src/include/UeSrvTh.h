/*!
 * \file UeSrvTh.h
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
#ifndef __UE_TH_H__
#define __UE_TH_H__

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
#include "UeSrvStreamTh.h"
#include "RobotSrvTh.h"

/**   2a.   External Functions                                      **/
/**   2b.   External Data                                           **/
/**   3.  Type Definitions                                          **/
/**   5.  Local Functions                                           **/
/**   6.  Macros / Defines                                          **/

#define MAX_RETRY 5
#define MAX_UE_CONNECTED 4

typedef enum eUeSrvThState {UE_TH_INIT=0, UE_TH_CONNECTED, UE_TH_RUNNING, UE_TH_STOPPED   } teUeSrvThState;

/*  see http://stackoverflow.com/questions/5150630/keeping-number-of-threads-constant-with-pthread-in-c */

/**
 * \class UeSrvThread
 * \brief 
 *  see http://stackoverflow.com/questions/5150630/keeping-number-of-threads-constant-with-pthread-in-c
 *  for manage thread
 *
 */
class UeSrvThreadCl
{
    public:
		UeSrvThreadCl(int, RobotSrvThreadCl *);

        teUeSrvThState state(void);

        static void *run(void *context)
        {
            return ((UeSrvThreadCl *)context)->_execute();
        }

        // bool addMsg(char*,uint32_t);
        // bool addMsg(tsMsgRobotSrv * );
        void stop(void);

        virtual ~UeSrvThreadCl();


    protected:
    private:
        bool                        _bContinue;
        bool                        _bStop;
        teUeSrvThState              _state;
        RtTrace                   * _trace;
        TCPAcceptor               * _acceptor;
        pthread_mutex_t             _mutexTh;
        pthread_mutex_t             _mutexFifo;
        TCPStream                 * _stream[MAX_UE_CONNECTED];
        pthread_t                   _ueThread[MAX_UE_CONNECTED];
        UeSrvStreamThreadCl       * _ueThreadObj[MAX_UE_CONNECTED];
        int                         _port;
        tsSocketStat                _stat;
        tsMsgRobotSrv               _tsMsgTx;
        tsMsgRobotSrv               _tsMsgRx;
        RobotSrvThreadCl          * _pts2Robot;

        // std::queue<tsMsgRobotSrv *> _TxFifo;

        // std::string __getHost(void);
        // int __getPort(void);

        void *_execute(void);
        
        // bool _mng_rx(void);
        // bool _mng_tx(void);
};

extern const std::string UE_TH_KEYS[];



#endif // __UE_TH_H__

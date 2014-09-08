/*!
 * \file UeSrvStreamThread.h
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
#ifndef __UE_STREAM_TH_H__
#define __UE_STREAM_TH_H__

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

#define MAX_RETRY 5


typedef enum eUeSrvStreamThreadState {UE_STREAM_TH_INIT=0, UE_STREAM_TH_CONNECTED, UE_STREAM_TH_RUNNING, UE_STREAM_TH_STOPPED   } teUeSrvStreamThreadState;

/*  see http://stackoverflow.com/questions/5150630/keeping-number-of-threads-constant-with-pthread-in-c */

extern pthread_cond_t     E_ueThreadCond ;
extern uint32_t           E_u32UeSrvStreamThJobs ;
extern pthread_mutex_t    E_ueSrvStreamMutex;
extern queue<uint32_t>    E_euThreadEndedFifo;

 /**
 * \class UeSrvStreamThreadCl
 * \brief 
 *  for manage thread
 *
 */
class UeSrvStreamThreadCl
{
    public:
		UeSrvStreamThreadCl(uint32_t, TCPStream *);

        teUeSrvStreamThreadState state(void);
        bool addMsg(tsMsgRobotSrv * );

        static void *run(void *context)
        {
            return ((UeSrvStreamThreadCl *)context)->_execute();
        }

         tsSocketStat stat(void);

        void stop(void);

        virtual ~UeSrvStreamThreadCl();


    protected:
    private:
        bool               _bContinue;
        bool               _bStop;
        teUeSrvStreamThreadState     _state;
        bool               _ended;
        uint32_t           _threadId;
        RtTrace          * _trace;
        TCPStream        * _stream;
        pthread_mutex_t    _mutexTh;
        pthread_mutex_t    _mutexFifo;

        tsSocketStat       _stat;
        message_t          _tsMsgRx;
        tsMsgRobotSrv      _tsMsgTx;
        // tsMsgRobotSrv      _tsMsgRx;

        std::queue<tsMsgRobotSrv *> _TxFifo;

        void *_execute(void);
        
        bool _mng_rx(void);
        bool _mng_tx(void);
};



#endif // __UE_STREAM_TH_H__

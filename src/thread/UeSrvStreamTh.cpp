/*!
 * \file UeSrvStreamTh.cpp
 * \author claude.bideau@gmail.com
 * \version 0.1
 * \brief This module provides a UeSrvStreamTh declaration.
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
#include <cassert>
#include <stdexcept>
#include <iostream>
#include <unistd.h>
#include <map>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include "trace.h"
#include "proto.h"
#include "UeSrvStreamTh.h"

using namespace std;

/*  see http://stackoverflow.com/questions/5150630/keeping-number-of-threads-constant-with-pthread-in-c */
pthread_cond_t     E_ueThreadCond =  PTHREAD_COND_INITIALIZER;
uint32_t           E_u32UeSrvStreamThJobs = 0;
pthread_mutex_t    E_ueSrvStreamMutex = PTHREAD_MUTEX_INITIALIZER;
queue<uint32_t>    E_euThreadEndedFifo;


static inline uint32_t Q_u32swap( uint32_t L_u32valueIn)
{
    uint32_t L_u32valueOut;
    uint8_t * L_pu8valueIn  = (uint8_t*) &L_u32valueIn;
    uint8_t * L_pu8valueOut = (uint8_t*) &L_u32valueOut;

    for ( uint8_t L_u8Idx =0; L_u8Idx< 4 ; L_u8Idx++)
    {
       L_pu8valueOut[L_u8Idx] = L_pu8valueIn[3-L_u8Idx];
    }
    return L_u32valueOut;
}




UeSrvStreamThreadCl::UeSrvStreamThreadCl(uint32_t F_u32JobId, TCPStream * F_ptsStream, RobotSrvThreadCl * F_pts2Robot)
{
    _threadId  = F_u32JobId;
    _pts2Robot = F_pts2Robot;
    _trace    = new RtTrace("ue", RT_TRACE_ENABLE);
    _trace->trace(0, RT_TRACE_ENABLE, 0xFFFFFFFF, 2);
	TRACES_INFO_ARG1("create UeSrvStreamThreadCl on thread %d",_threadId );
    Q_vInitStat(&_stat);
    pthread_mutex_init(&_mutexTh,NULL);
    pthread_mutex_init(&_mutexFifo,NULL);
    _stream = F_ptsStream;

    _stream->setnonblock();
    memset(&_tsMsgTx,0,sizeof(tsMsgRobotSrv));
    _stat.connected++;
    
    pthread_mutex_lock(&_mutexTh);
    _bContinue = false;
    _bStop     = false;
    _ended     = false;
    pthread_mutex_unlock(&_mutexTh);
	
}

 
/**
   @brief addMsg to Ue
   @return none.
 */
bool UeSrvStreamThreadCl::addMsg(tsMsgRobotSrv * F_ptsMsg )
{ 
    // tsMsgRobotSrv * L_ptsMsg = (tsMsgRobotSrv *) malloc(sizeof(tsMsgRobotSrv));
   
    // memcpy((void*) L_ptsMsg,(void*)F_ptsMsg,sizeof(tsMsgRobotSrv));
    // pthread_mutex_lock(&_mutexFifo);
    // _TxFifo.push (L_ptsMsg);
    // pthread_mutex_unlock(&_mutexFifo);
    return true;
}


/**
   @brief request to stop socket thread
   @return none.
 */
void UeSrvStreamThreadCl::stop(void)
{ 
    pthread_mutex_lock(&_mutexTh);
    _bContinue = false;
    _bStop     = false;
    pthread_mutex_unlock(&_mutexTh);
    return;
}



void *UeSrvStreamThreadCl::_execute(void)
{
    bool L_bLoop;
    int L_i32Idx = 0;


    TRACES_INFO_ARG2("execute core of UeSrvStreamThreadCl on thread %d  [job==%d]",_threadId, E_u32UeSrvStreamThJobs);
    pthread_mutex_lock(&_mutexTh);
    _bStop  = true;
    L_bLoop = _bStop;
    pthread_mutex_unlock(&_mutexTh);
    
    while (L_bLoop ==true)
    {
   
        /* wait the start of socket communication stream */

        pthread_mutex_lock(&_mutexTh);
        _bContinue = true;
        L_bLoop    = (_bStop==true);
        pthread_mutex_unlock(&_mutexTh);
        if (L_bLoop  == true)
        {
            L_bLoop &= _mng_rx();
            L_bLoop &= _mng_tx();
            sleep(1);
        }
    }
    cout << "_stat.connected = " << _stat.connected << endl;
    cout << "_stat.retry     = " << _stat.retry     << endl;
    cout << "_stat.tx        = " << _stat.tx        << endl;
    cout << "_stat.txbytes   = " << _stat.txbytes   << endl;
    cout << "_stat.rx        = " << _stat.rx        << endl;
    cout << "_stat.rxbytes   = " << _stat.rxbytes   << endl;
    
    TRACES_INFO("end of core of UeSrvStreamThreadCl");
    /* inform that stream is ended */
    pthread_mutex_lock(&_mutexTh);
    E_euThreadEndedFifo.push(_threadId);
    E_u32UeSrvStreamThJobs--;
    pthread_cond_signal(&E_ueThreadCond);
    _ended = true;
    pthread_mutex_unlock(&_mutexTh);

    return 0;
}

bool UeSrvStreamThreadCl::_mng_rx(void)
{
    bool L_bLoop  =true;
    int L_i32lengthRx;
    static uint32_t L_u32lastTxId;
//
    L_i32lengthRx = _stream->receive((char *) &_tsMsgRx, sizeof(message_t), 1000);
    if ( L_i32lengthRx > 0) 
    {
        /* core of ue server feature   */
        /* - determine type of message    */
        /* => call appropriate function   */
        _stat.rx ++;
        _stat.rxbytes += L_i32lengthRx;
       
#if 0
       {
            uint32_t * L_pu32aBuffer = (uint32_t*) &_tsMsgRx;
            for (int i=0;i<sizeof(message_t)/4;i++)
            {
                printf("rx [%d][%d] = %08x\n",_stat.rx,i,L_pu32aBuffer[i]);
            }
        }
#endif
        if (PROTO_VERSION == _tsMsgRx.version)
        {
            /* what to do ? */
            switch ( _tsMsgRx.type ) 
            {
                case T_COMMAND: /* no supported */
                    break;
                case T_DATA_WII: /* no supported */
                    break;
                case T_DATA_JOY: /* joystick - not supported */
                    break;
                case T_DATA_AND:  /* android - supported */
                    if ( _pts2Robot != NULL)
                    {
                        try {
                            /* swap */

                            uint16_t L_u16v1 = (uint16_t) Q_u32swap((uint32_t)_tsMsgRx.pl.android.azimuth) &0xFFFF;
                            uint16_t L_u16v2 = (uint16_t) Q_u32swap((uint32_t)_tsMsgRx.pl.android.pitch) &0xFFFF;
                            uint16_t L_u16v3 = (uint16_t) Q_u32swap((uint32_t)_tsMsgRx.pl.android.roll) & 0xFFFF;
                            uint16_t L_u16b  = (uint16_t) _tsMsgRx.pl.android.buttons1 & 0xFFFF;
                            //uint16_t L_u16b1  = floor(L_u16b2/256);
                            //L_u16b2=L_u16b2-L_u16b1*256;
                            tsUePayload L_tsUePayload = {(int16_t)L_u16b,(int16_t)L_u16v1, (int16_t)L_u16v2, (int16_t)L_u16v3};
                            //cout<<"b: "<<L_u16b<<endl;
                            //cout<<"v1: "<<L_u16v1<<endl;
                            //cout<<"v2: "<<L_u16v2<<endl;
                            //cout<<"v3: "<<L_u16v3<<endl;
                            _pts2Robot->addData(&L_tsUePayload);

                        }
                        catch(std::string &error)
                        {
                            cerr<<"UeSrvStreamThreadCl : Error: "<<error<<endl;
                        }
                        catch(char* error)
                        {
                            cerr<<"UeSrvStreamThreadCl : Error: "<<error<<endl;
                            
                        }
                    }
                    break;
            }
           // uint8_t L_msgid = _tsMsgRx.header.type & SRV_OPE;
           // bool  L_ack     = ( SRV_ACK ==(_tsMsgRx.header.type & SRV_AN_MASK));
//            switch (L_msgid)
//            {
//                case UE_INIT :
//                    _tsMsgTx.header.version = PROTO_VERSION;
//                    _tsMsgTx.header.size    = 0;
//                    _tsMsgTx.header.type    = UE_INIT | SRV_ACK;
//                    _tsMsgTx.header.state   = 0;
//                    _tsMsgTx.header.rxid    = _tsMsgRx.header.txid;
//                    
//                    L_u32lastTxId = _tsMsgTx.header.txid;
//                    addMsg(&_tsMsgTx);
//                    break;
//                case UE_CONNECT :
//                    _tsMsgTx.header.version = PROTO_VERSION;
//                    _tsMsgTx.header.size    = 0;
//                    _tsMsgTx.header.type    = UE_CONNECT | SRV_ACK;
//                    _tsMsgTx.header.state   = 0;
//                    _tsMsgTx.header.rxid    = _tsMsgRx.header.txid;
//
//                    L_u32lastTxId = _tsMsgTx.header.txid;
//                    addMsg(&_tsMsgTx);
//
//                    break;
//                case UE_KEEPALIVE:
//                    /* no action if traffic occurred */
//                    if ( L_u32lastTxId == _tsMsgTx.header.txid )
//                    {
//                        /* send keepalive */
//                        _tsMsgTx.header.version = PROTO_VERSION;
//                        _tsMsgTx.header.size    = 0;
//                        _tsMsgTx.header.type    = UE_KEEPALIVE | SRV_ACK;
//                        _tsMsgTx.header.state   = 0;
//                        _tsMsgTx.header.rxid    = _tsMsgRx.header.txid;
//
//                        addMsg(&_tsMsgTx);
//                        cout << "k" <<endl;
//
//                    }
//                    L_u32lastTxId = _tsMsgTx.header.txid;
//                    break;
//                default:
//                    throw std::string("not a compatible socket");
//                    break;
//            }
        }
//
//        // _stream->send((char *)L_u16aBufferTx, L_i32lengthTx);
        /* end of ue server feature */
    } else if (L_i32lengthRx == 0)
    {
        pthread_mutex_lock(&_mutexTh);
        _bStop     = false;
        L_bLoop    = false;
        pthread_mutex_unlock(&_mutexTh);
        cout <<"EOT UeSrvStreamThreadCl" <<endl;
    }
    return L_bLoop;
}

bool UeSrvStreamThreadCl::_mng_tx(void)
{
    bool L_bLoop =true;
//    uint32_t L_u32size;
//    tsMsgRobotSrv * L_ptsMsgRobotSrv;
//    if (!_TxFifo.empty())
//    {
//        pthread_mutex_lock(&_mutexFifo);
//        L_ptsMsgRobotSrv = _TxFifo.front();
//        if (L_ptsMsgRobotSrv!=NULL)
//        {
//            L_u32size =  sizeof(tsMsgRobotSrvHeader)+L_ptsMsgRobotSrv->header.size;
//#ifdef __TEST
//            {
//                uint32_t * L_pu32aBuffer = (uint32_t*) L_ptsMsgRobotSrv;
//                for (int i=0;i<L_u32size/4;i++)
//                {
//                    printf("tx %08x\n",L_pu32aBuffer[i]);
//                }
//            }
//#endif            
//            _tsMsgTx.header.txid ++;
//            _stream->send((char *)L_ptsMsgRobotSrv,L_u32size );
//            _stat.tx ++;
//            _stat.txbytes += L_u32size;
//            free(L_ptsMsgRobotSrv);
//        }
//        _TxFifo.pop();
//        pthread_mutex_unlock(&_mutexFifo);
//    }
  
    return L_bLoop;
}


UeSrvStreamThreadCl::~UeSrvStreamThreadCl()
{
    bool L_ended;
    stop();

    /* wait until stream and _acceptor deleted */
    {
        pthread_mutex_lock(&_mutexTh);
        L_ended = _ended;
        pthread_mutex_unlock(&_mutexTh);
        if (L_ended == false) sleep(1);
    } while ( L_ended == false );

    std::cout << "~UeSrvStreamThreadCl : delete done" << endl;
}


/*!
 * \file UeSrvTh.cpp
 * \author claude.bideau@gmail.com
 * \version 0.1
 * \brief This module provides a UeSrvTh declaration.
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

#include "trace.h"
#include "proto.h"
#include "UeSrvTh.h"

using namespace std;



UeSrvThreadCl::UeSrvThreadCl(int F_i32port, RobotSrvThreadCl * F_pts2Robot)
{
    _port         = F_i32port;
    _pts2Robot    = F_pts2Robot;
    TRACES_INFO_ARG1("create UeSrvThreadCl on port [%d]",_port );

    _acceptor = new TCPAcceptor(_port);
    if (_acceptor->start() != 0) throw std::string("UeSrvThreadCl : impossible to start socket");
    
    Q_vInitStat(&_stat);
    pthread_mutex_init(&_mutexTh,NULL);
    pthread_mutex_init(&_mutexFifo,NULL);
    pthread_mutex_init(&E_ueSrvStreamMutex,NULL);
    for(int i=0; i < MAX_UE_CONNECTED; i++)
    {
        _stream[i] = NULL;
        _ueThreadObj[i] = NULL;
    }
    
    pthread_mutex_lock(&_mutexTh);
    _bContinue = false;
    _bStop     = false;
    pthread_mutex_unlock(&_mutexTh);
	
}


// /**
   // @brief addMsg to Ue
   // @return none.
 // */
// bool UeSrvThreadCl::addMsg(tsMsgRobotSrv * F_ptsMsg )
// { 
    // tsMsgRobotSrv * L_ptsMsg = (tsMsgRobotSrv *) malloc(sizeof(tsMsgRobotSrv));
   
    // memcpy((void*) L_ptsMsg,(void*)F_ptsMsg,sizeof(tsMsgRobotSrv));
    // pthread_mutex_lock(&_mutexFifo);
    // _TxFifo.push (L_ptsMsg);
    // pthread_mutex_unlock(&_mutexFifo);
    // return true;
// }


/**
   @brief request to stop socket thread
   @return none.
 */
void UeSrvThreadCl::stop(void)
{ 
    pthread_mutex_lock(&_mutexTh);
    _bContinue = false;
    _bStop     = false;
    for (int i=0; i < MAX_UE_CONNECTED; i++)
    {
        if (_ueThreadObj[i] != NULL) _ueThreadObj[i]->stop();
        pthread_join(_ueThread[i], NULL);
        delete _ueThreadObj[i];
        _ueThreadObj[i]=NULL;
    }
    pthread_mutex_unlock(&_mutexTh);
    return;
}



void *UeSrvThreadCl::_execute(void)
{
    bool L_bLoop;
    uint32_t L_u32Idx  = 0;
    int L_i32IdxUeThread = 0;
    tsSocketStat  L_tsStat;


    TRACES_INFO_ARG1("execute core of UeSrvThreadCl on port [%d]",_port );
    pthread_mutex_lock(&_mutexTh);
    _bStop  = true;
    L_bLoop = _bStop;
    pthread_mutex_unlock(&_mutexTh);
    
    while (L_bLoop ==true)
    {
        _bContinue = true;
   
        /* critical section */
        /* wait condition   */
        // cout << "UeSrvThreadCl : Enter in critical section"<<endl;
        pthread_mutex_lock(&E_ueSrvStreamMutex);
        
        // cout << "UeSrvThreadCl : nb jobs =" << E_u32UeSrvStreamThJobs << endl;
        while (E_u32UeSrvStreamThJobs >= MAX_UE_CONNECTED)
        {
            // cout << "UeSrvThreadCl : condition wait [" << E_u32UeSrvStreamThJobs << "]" <<endl;
            pthread_cond_wait(&E_ueThreadCond, &E_ueSrvStreamMutex);
        }
        /* dequeue to determine the terminate thread(s) and delete context */
        // cout << "Dequeue :? " <<endl;
        while (!E_euThreadEndedFifo.empty())
        {
            /* if thread ended fifo not empty dequeue all task id */
            L_u32Idx = E_euThreadEndedFifo.front();
            cout << __LINE__<< " : join and terminate stream thread ["<< L_u32Idx<<"]" <<endl;
            pthread_join(_ueThread[L_u32Idx], NULL);
            L_tsStat = _ueThreadObj[L_u32Idx]->stat();
            Q_vAddStat(&_stat,&L_tsStat);
            _ueThreadObj[L_u32Idx]->stop();
            delete _ueThreadObj[L_u32Idx];
             _ueThreadObj[L_u32Idx]=NULL;
            delete _stream[L_u32Idx];
            E_euThreadEndedFifo.pop();
        }
        
        /* determine next ue thread id */
        L_i32IdxUeThread = -1;
        // cout << "find thread id" <<endl;
        for(int i=0; i < MAX_UE_CONNECTED; i++)
        {
            if (_ueThreadObj[i] == NULL)
            {
                L_i32IdxUeThread = i;
                break;
            }
        }
        // cout << __LINE__<< " : run on ["<< L_i32IdxUeThread << "]" <<endl;
        pthread_mutex_unlock(&E_ueSrvStreamMutex);
        // cout << __LINE__<< " : UeSrvThreadCl : End of critical section"<<endl;

        cout << __LINE__<< " : accept ?"<<endl;

        /* wait the start of socket communication stream */
        _stream[L_i32IdxUeThread] = _acceptor->accept();
        if (_stream[L_i32IdxUeThread] != NULL)
        {
            _stream[L_i32IdxUeThread]->setnonblock();
            pthread_mutex_lock(&E_ueSrvStreamMutex);
            E_u32UeSrvStreamThJobs++;
            pthread_mutex_unlock(&E_ueSrvStreamMutex);
            /* create UeSrvStreamThreadCl */
            _ueThreadObj[L_i32IdxUeThread] = new UeSrvStreamThreadCl(L_i32IdxUeThread,_stream[L_i32IdxUeThread],_pts2Robot);
            
            cout << "UeSrvThreadCl : thread creation" <<endl;
            pthread_create(&_ueThread[L_i32IdxUeThread], NULL, &UeSrvStreamThreadCl::run, _ueThreadObj[L_i32IdxUeThread]);     
            
            _stat.connected++;

            pthread_mutex_lock(&_mutexTh);
            _bContinue = true;
            L_bLoop = (_bStop==true);
            pthread_mutex_unlock(&_mutexTh);
        }

        pthread_mutex_lock(&_mutexTh);
        L_bLoop = _bStop;
        pthread_mutex_unlock(&_mutexTh);
        cout << "UeSrvThreadCl : _stat.connected = " << _stat.connected << endl;
        // cout << "_stat.retry     = " << _stat.retry     << endl;
        // cout << "_stat.tx        = " << _stat.tx        << endl;
        // cout << "_stat.txbytes   = " << _stat.txbytes   << endl;
        // cout << "_stat.rx        = " << _stat.rx        << endl;
        // cout << "_stat.rxbytes   = " << _stat.rxbytes   << endl;
    }
    delete _acceptor;
    _acceptor=NULL;
    
    TRACES_INFO("end of core of UeSrvThreadCl");
    return 0;
}

// bool UeSrvThreadCl::_mng_rx(void)
// {
    // bool L_bLoop  =true;
//    int L_i32lengthRx;
//    static uint32_t L_u32lastTxId;
//
//    L_i32lengthRx = _stream->receive((char *) &_tsMsgRx, sizeof(tsMsgRobotSrv), 2);
//    if ( L_i32lengthRx > 0) 
//    {
//        /* core of robot server feature   */
//        /* - determine type of message    */
//        /* => call appropriate function   */
//        _stat.rx ++;
//        _stat.rxbytes += L_i32lengthRx;
//        
//        if (PROTO_VERSION == _tsMsgRx.header.version)
//        {
//            uint8_t L_msgid = _tsMsgRx.header.type & SRV_OPE;
//            bool  L_ack     = ( SRV_ACK ==(_tsMsgRx.header.type & SRV_AN_MASK));
//#ifdef __TEST
//            {
//                uint32_t * L_pu32aBuffer = (uint32_t*) &_tsMsgRx;
//                for (int i=0;i<sizeof(tsMsgRobotSrv)/4;i++)
//                {
//                    printf("rx [%d] = %08x\n",i,L_pu32aBuffer[i]);
//                }
//            }
//#endif
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
//        }
//
//        // _stream->send((char *)L_u16aBufferTx, L_i32lengthTx);
//        /* end of robot server feature */
//    } else if (L_i32lengthRx == 0)
//    {
//        pthread_mutex_lock(&_mutexTh);
//        _bContinue = false;
//        L_bLoop = false;
//        pthread_mutex_unlock(&_mutexTh);
//        cout <<"EOT" <<endl;
//    }
    // return L_bLoop;
// }

// bool UeSrvThreadCl::_mng_tx(void)
// {
    // bool L_bLoop =true;
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
//  
    // return L_bLoop;
// }


UeSrvThreadCl::~UeSrvThreadCl()
{
    stop();

    /* wait until stream and _acceptor deleted */
    while ( _acceptor != NULL) sleep(1);

    std::cout << "~UeSrvThreadCl : delete done" << endl;
}


/*!
 * \file RobotSrvTh.cpp
 * \author claude.bideau@gmail.com
 * \version 0.1
 * \brief This module provides a RobotSrvTh declaration.
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

#include "IssueException.h"
#include "RobotSrvTh.h"
#include "trace.h"
#include "proto.h"

using namespace std;



RobotSrvThreadCl::RobotSrvThreadCl(int F_i32port)
{
    _port     = F_i32port;
    TRACES_INFO_ARG1("create RobotSrvThreadCl on port [%d]",_port );
    _acceptor = new TCPAcceptor(_port);
    if (_acceptor->start() != 0) throw std::string("impossible to start socket");
    
    Q_vInitStat(&_stat);
    pthread_mutex_init(&_mutexTh,NULL);
    pthread_mutex_init(&_mutexFifo,NULL);
    
    pthread_mutex_lock(&_mutexTh);
    _bContinue = false;
    _bStop     = false;
    pthread_mutex_unlock(&_mutexTh);
	
}

/**
   @brief insert Message to tx FIFO 
   @return none.
 */
bool RobotSrvThreadCl::addMsg(tsMsgRobotSrv * F_ptsMsg )
{ 
    tsMsgRobotSrv * L_ptsMsg = (tsMsgRobotSrv *) malloc(sizeof(tsMsgRobotSrv));
   
    memcpy((void*) L_ptsMsg,(void*)F_ptsMsg,sizeof(tsMsgRobotSrv));
    pthread_mutex_lock(&_mutexFifo);
    _TxFifo.push (L_ptsMsg);
    pthread_mutex_unlock(&_mutexFifo);
    return true;
}

/**
   @brief insert data from Ue to Tx Fifo
   @return none.
 */
bool RobotSrvThreadCl::addData(tsUePayload * F_ptsData )
{ 
    tsMsgRobotSrv * L_ptsMsg = (tsMsgRobotSrv *) malloc(sizeof(tsMsgRobotSrv));
    /* populate data */
    L_ptsMsg->header.version = PROTO_VERSION;   /**< protocol version */
    L_ptsMsg->header.size    = sizeof(tsUePayload);      /**< message size */
    L_ptsMsg->header.type    = ROBOT_DATA;      /**< message type. will define the payload */
    L_ptsMsg->header.state   = 0;
    L_ptsMsg->header.txid    = 0;
    L_ptsMsg->header.rxid    = 0;
    L_ptsMsg->pl.ue = (*F_ptsData);
    pthread_mutex_lock(&_mutexFifo);
    if (MAX_TX_FIFO_SIZE > _TxFifo.size())
    {
        _TxFifo.push (L_ptsMsg);
        cout << "add 2 tx"<<endl;
    }
    else cout << "D";
    pthread_mutex_unlock(&_mutexFifo);
    return true;
}

/**
   @brief request to stop socket thread
   @return none.
 */
void RobotSrvThreadCl::stop(void)
{ 
    pthread_mutex_lock(&_mutexTh);
    _bContinue = false;
    _bStop     = false;
    pthread_mutex_unlock(&_mutexTh);
    return;
}


/**
   @brief core of the thread running
   @return none.
 */
void *RobotSrvThreadCl::_execute(void)
{
    bool L_bLoop;
    int L_i32Idx = 0;


    TRACES_INFO_ARG1("execute core of RobotSrvThreadCl on port [%d]",_port );
    pthread_mutex_lock(&_mutexTh);
    _bStop  = true;
    L_bLoop = _bStop;
    pthread_mutex_unlock(&_mutexTh);
    
    while (L_bLoop ==true)
    {
        _bContinue = true;
   
        /* wait the start of socket communication stream */
        cout << "accept ?"<<endl;
        _stream = _acceptor->accept();
        if (_stream != NULL)
        {
            /* cleanup tx fifo */
            while (!_TxFifo.empty())
            {
                cout << "RobotSrvThreadCl : get from  2 tx fifo" << this << endl;

                pthread_mutex_lock(&_mutexFifo);
                _TxFifo.pop();
                pthread_mutex_unlock(&_mutexFifo);
                cout << "-";
            }

            /* set stream non block */
            _stream->setnonblock();
            memset(&_tsMsgTx,0,sizeof(tsMsgRobotSrv));
            _stat.connected++;

            pthread_mutex_lock(&_mutexTh);
            _bContinue = true;
            L_bLoop = _bContinue==true && _bStop==true;
            pthread_mutex_unlock(&_mutexTh);
            while ( L_bLoop )
            {
                L_bLoop  = _mng_rx();
                L_bLoop &= _mng_tx();
            }
            delete _stream;
        }
        _stream =NULL;
        pthread_mutex_lock(&_mutexTh);
        L_bLoop = _bStop;
        pthread_mutex_unlock(&_mutexTh);
        cout << "_stat.connected = " << _stat.connected << endl;
        cout << "_stat.retry     = " << _stat.retry     << endl;
        cout << "_stat.tx        = " << _stat.tx        << endl;
        cout << "_stat.txbytes   = " << _stat.txbytes   << endl;
        cout << "_stat.rx        = " << _stat.rx        << endl;
        cout << "_stat.rxbytes   = " << _stat.rxbytes   << endl;
    }
    delete _acceptor;
    _acceptor=NULL;
    
    TRACES_INFO("end of core of RobotSrvThreadCl");
    return 0;
}

bool RobotSrvThreadCl::_mng_rx(void)
{
    bool L_bLoop  =true;
    int L_i32lengthRx;
    static uint32_t L_u32lastTxId;

    L_i32lengthRx = _stream->receive((char *) &_tsMsgRx, sizeof(tsMsgRobotSrv), 1000);
    if ( L_i32lengthRx > 0) 
    {
        /* core of robot server feature   */
        /* - determine type of message    */
        /* => call appropriate function   */
        _stat.rx ++;
        _stat.rxbytes += L_i32lengthRx;
        
        if (PROTO_VERSION == _tsMsgRx.header.version)
        {
            uint8_t L_msgid = _tsMsgRx.header.type & SRV_OPE;
            bool  L_ack     = ( SRV_ACK ==(_tsMsgRx.header.type & SRV_AN_MASK));
#ifdef __TEST
            {
                uint32_t * L_pu32aBuffer = (uint32_t*) &_tsMsgRx;
                for (int i=0;i<sizeof(tsMsgRobotSrv)/4;i++)
                {
                    printf("rx [%d] = %08x\n",i,L_pu32aBuffer[i]);
                }
            }
#endif
            switch (L_msgid)
            {
                case ROBOT_INIT :
                    _tsMsgTx.header.version = PROTO_VERSION;
                    _tsMsgTx.header.size    = 0;
                    _tsMsgTx.header.type    = ROBOT_INIT | SRV_ACK;
                    _tsMsgTx.header.state   = 0;
                    _tsMsgTx.header.rxid    = _tsMsgRx.header.txid;
                    
                    L_u32lastTxId = _tsMsgTx.header.txid;
                    addMsg(&_tsMsgTx);
                    break;
                case ROBOT_CONNECT :
                    _tsMsgTx.header.version = PROTO_VERSION;
                    _tsMsgTx.header.size    = 0;
                    _tsMsgTx.header.type    = ROBOT_CONNECT | SRV_ACK;
                    _tsMsgTx.header.state   = 0;
                    _tsMsgTx.header.rxid    = _tsMsgRx.header.txid;

                    L_u32lastTxId = _tsMsgTx.header.txid;
                    addMsg(&_tsMsgTx);

                    break;
                case ROBOT_KEEPALIVE:
                    /* no action if traffic occurred */
                    if ( L_u32lastTxId == _tsMsgTx.header.txid )
                    {
                        /* send keepalive */
                        _tsMsgTx.header.version = PROTO_VERSION;
                        _tsMsgTx.header.size    = 0;
                        _tsMsgTx.header.type    = ROBOT_KEEPALIVE | SRV_ACK;
                        _tsMsgTx.header.state   = 0;
                        _tsMsgTx.header.rxid    = _tsMsgRx.header.txid;

                        addMsg(&_tsMsgTx);
                        cout << "k";

                    }
                    L_u32lastTxId = _tsMsgTx.header.txid;
                    break;
                default:
                    /* throw std::string("not a compatible socket");*/
                    throw IssueCl(1,"not a compatible socket");
                    break;
            }
        }

        // _stream->send((char *)L_u16aBufferTx, L_i32lengthTx);
        /* end of robot server feature */
    } else if (L_i32lengthRx == 0)
    {
        pthread_mutex_lock(&_mutexTh);
        _bContinue = false;
        L_bLoop = false;
        pthread_mutex_unlock(&_mutexTh);
        cout <<"EOT" <<endl;
    }
    return L_bLoop;
}

bool RobotSrvThreadCl::_mng_tx(void)
{
    tsQueueBuffer L_tsQueueBuffer;
    bool L_bLoop =true;
    uint32_t L_u32size;
    tsMsgRobotSrv * L_ptsMsgRobotSrv;
    while (!_TxFifo.empty())
    {
        pthread_mutex_lock(&_mutexFifo);
        L_ptsMsgRobotSrv = _TxFifo.front();
        if (L_ptsMsgRobotSrv!=NULL)
        {
            L_u32size =  sizeof(tsMsgRobotSrvHeader)+L_ptsMsgRobotSrv->header.size;
#if 0
            {
                uint32_t * L_pu32aBuffer = (uint32_t*) L_ptsMsgRobotSrv;
                for (int i=0;i<L_u32size/4;i++)
                {
                    printf("tx %08x\n",L_pu32aBuffer[i]);
                }
            }
#endif
            _tsMsgTx.header.txid ++;
            _stream->send((char *)L_ptsMsgRobotSrv,L_u32size );
            _stat.tx ++;
            _stat.txbytes += L_u32size;
            free(L_ptsMsgRobotSrv);
        }
        _TxFifo.pop();
        pthread_mutex_unlock(&_mutexFifo);
    }
  
    return L_bLoop;
}


RobotSrvThreadCl::~RobotSrvThreadCl()
{
    stop();

    /* wait until stream and _acceptor deleted */
    while ( _acceptor != NULL) sleep(1);

    std::cout << "~RobotSrvThreadCl : delete done" << endl;
}


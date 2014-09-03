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
   @brief request to stop socket thread
   @return none.
 */
bool RobotSrvThreadCl::addMsg(char* F_pcaMsg,uint32_t F_u32size)
{ 

    if (F_u32size==0) return false;
    tsQueueBuffer L_tsQueueBuffer;
    L_tsQueueBuffer.size=F_u32size;
    memcpy((void*) &L_tsQueueBuffer.buffer,(void*)F_pcaMsg,F_u32size*sizeof(char));
    pthread_mutex_lock(&_mutexFifo);
    _TxFifo.push (L_tsQueueBuffer);
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

    L_i32lengthRx = _stream->receive((char *) &_tsMsgRx, sizeof(tsMsgRobotSrv), 2);
    if ( L_i32lengthRx > 0) 
    {
        /* core of robot server feature   */
        /* - determine type of message    */
        /* => call appropriate function   */
        _stat.rx ++;
        _stat.rxbytes += L_i32lengthRx;
        
        if (PROTO_VERSION == _tsMsgRx.version)
        {
            uint8_t L_msgid = _tsMsgRx.type & SRV_OPE;
            bool  L_ack     = ( SRV_ACK ==(_tsMsgRx.type & SRV_AN_MASK));
            {
                uint32_t * L_pu32aBuffer = (uint32_t*) &_tsMsgRx;
                for (int i=0;i<sizeof(tsMsgRobotSrv)/4;i++)
                {
                    printf("rx [%d] = %08x\n",i,L_pu32aBuffer[i]);
                }
            }
            switch (L_msgid)
            {
                case ROBOT_INIT :
                    _tsMsgTx.version = PROTO_VERSION;
                    _tsMsgTx.size    = 0;
                    _tsMsgTx.type    = ROBOT_INIT | SRV_ACK;
                    _tsMsgTx.state   = 0;
                    _tsMsgTx.txid ++;
                    _tsMsgTx.rxid    = _tsMsgRx.txid;
                    
                    addMsg((char*) &_tsMsgTx, sizeof(tsMsgRobotSrv));
                    break;
                case ROBOT_CONNECT :
                    _tsMsgTx.version = PROTO_VERSION;
                    _tsMsgTx.size    = 0;
                    _tsMsgTx.type    = ROBOT_CONNECT | SRV_ACK;
                    _tsMsgTx.state   = 0;
                    _tsMsgTx.txid ++;
                    _tsMsgTx.rxid    = _tsMsgRx.txid;

                    addMsg((char*) &_tsMsgTx, sizeof(tsMsgRobotSrv));

                    break;
                default:
                    throw std::string("not a compatible socket");
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
    memset(&L_tsQueueBuffer,0,sizeof(tsQueueBuffer));
    if (!_TxFifo.empty())
    {
        pthread_mutex_lock(&_mutexFifo);
        L_tsQueueBuffer = _TxFifo.front();
        if (L_tsQueueBuffer.size > 0)
        {
            {
                uint32_t * L_pu32aBuffer = (uint32_t*) &L_tsQueueBuffer.buffer;
                for (int i=0;i<L_tsQueueBuffer.size/4;i++)
                {
                    printf("%08x\n",L_pu32aBuffer[i]);
                }
            }
            _stream->send((char *)&L_tsQueueBuffer.buffer, L_tsQueueBuffer.size);
            _TxFifo.pop();
            _stat.tx ++;
            _stat.txbytes += L_tsQueueBuffer.size;
        }
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


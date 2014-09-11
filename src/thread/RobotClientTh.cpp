/*!
 * \file RobotClientTh.cpp
 * \author claude.bideau@gmail.com
 * \version 0.1
 * \brief This module provides a RobotClientTh declaration.
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
#include "RobotClientTh.h"

using namespace std;



RobotClientThreadCl::RobotClientThreadCl(iniCl * _pIni )
{
    _state    = RC_INIT;
    /* ============================================== */
    std::list<std::string> L_listKey;
    
    
	L_listKey = _pIni->getItems("main");
	if (!L_listKey.empty())
	{
        _port  = _pIni->getInteger("main","port", -1 );
        _host  = _pIni->get("main","host" );
        if ((_port == -1) || (_host.empty())) 
            throw std::string("The socket port has not been set in ini file (section [main] / host and or port )!!!");
    } else {
            throw std::string("The socket port has not been set in ini file (section (main] / host and or port )!!!");
    }
    /* ============================================== */

    Q_vInitStat(&_stat);
    TRACES_INFO_ARG2("create RobotClientThreadCl on port [%s][%d]",_host.c_str(), _port );
    _connector = new TCPConnector();
    pthread_mutex_init(&_mutex,NULL);
    
    pthread_mutex_lock(&_mutex);
    _bContinue = false;
    _bStop     = false;
    pthread_mutex_unlock(&_mutex);
	
}

/**
   @brief request to stop socket thread
   @return none.
 */
void RobotClientThreadCl::stop(void)
{ 
    cout << "RobotClientThreadCl : stop requested" << endl;
    pthread_mutex_lock(&_mutex);
    _bContinue = false;
    _bStop     = false;
    pthread_mutex_unlock(&_mutex);
    return;
}



void *RobotClientThreadCl::_execute(void)
{
    bool L_bLoop;
    int L_i32Idx = 0;
    int L_i32lengthRx;
    bool  L_bSendDone=false;
    unsigned short L_u16aBufferRx[1024];

    TRACES_INFO_ARG1("execute core of RobotClientThreadCl on port [%d]",_port );
    pthread_mutex_lock(&_mutex);
    _bStop  = true;
    L_bLoop = _bStop;
    pthread_mutex_unlock(&_mutex);
    
    while (L_bLoop ==true)
    {
   
        /* wait the start of socket communication stream */
        _stream = _connector->connect(_host.c_str(), _port);
        if (_stream != NULL)
        {
            _stream->setnonblock();
            memset(&_tsMsgTx,0,sizeof(tsMsgRobotSrv));
            cout << "non block\n";
            _state    = RC_INIT;
            _stat.connected++;
            L_bSendDone=false;

            pthread_mutex_lock(&_mutex);
            _bContinue = true;
            L_bLoop = _bContinue==true && _bStop==true;
            pthread_mutex_unlock(&_mutex);
            while ( L_bLoop )
            {

                _tsMsgTx.header.version = PROTO_VERSION;
                _tsMsgTx.header.size    = 0;
                
                /* non blocking mode   */
                /* wait Rx packet      */
                L_i32lengthRx = _stream->receive((char *) &_tsMsgRx,  sizeof(tsMsgRobotSrv),1000);
                if (L_i32lengthRx == 0)
                {
                    /* connection close */
                    L_bLoop = false;
                    cout <<"/\n";
                    break;
                } else if (L_i32lengthRx > 0 )
                {
                    _stat.rx ++; 
                    _stat.rxbytes += L_i32lengthRx;
                    
                    /* manage Rx packet */
#ifdef __TEST
                    cout << "rx packet     = " <<L_i32lengthRx<<endl;
                    cout << "_tsMsgRx.header.type = " <<_tsMsgRx.header.type<<endl;
                    {
                        uint32_t * L_pu32aBuffer = (uint32_t*) &_tsMsgRx;
                        for (int i=0;i<L_i32lengthRx/4;i++)
                        {
                            printf("rx %08x\n",L_pu32aBuffer[i]);
                        }
                    }
#endif
                    if (PROTO_VERSION == _tsMsgRx.header.version)
                    {
                        uint8_t L_msgid = _tsMsgRx.header.type & SRV_OPE;
                        bool  L_ack     = ( SRV_ACK ==(_tsMsgRx.header.type & SRV_AN_MASK));
                        cout << "yes check" <<endl;
                        
                        switch (L_msgid)
                        {
                            case ROBOT_INIT :
                                cout << "yes ROBOT_INIT check" << _state <<endl;
                            
                                if ((_state == RC_INIT) && L_ack)
                                {
                                    L_bSendDone=false;
                                    _state = RC_CONNECTED;
                                }
                                break;
                            case ROBOT_CONNECT :
                                cout << "yes ROBOT_CONNECT check" << _state <<endl;
                                if ((_state == RC_CONNECTED) && ( SRV_ACK ==(_tsMsgRx.header.type & SRV_AN_MASK) ))
                                {
                                    L_bSendDone==false;
                                    _state = RC_RUNNING;
                                } else {
                                    L_bSendDone=false;
                                    _state = RC_INIT;
                                }

                                break;
                            case ROBOT_KEEPALIVE :
                                /* nothing */
                                break;
                            case ROBOT_REINIT:
                                if (_state != RC_RUNNING) 
                                {
                                    /* reject */
                                    _tsMsgTx.header.type  = ROBOT_REINIT | SRV_NACK;
                                } else {
                                    /* accepted send ack => change state to RC_CALIBRATE_REQ */
                                    _tsMsgTx.header.type  = ROBOT_REINIT | SRV_ACK;
                                    _state = RC_CALIBRATE_REQ;
                                }
                                _tsMsgTx.header.state = (uint8_t) _state;
                                _tsMsgTx.header.rxid = _tsMsgRx.header.txid;
                                /* send packet */
                                _stream->send((char *)&_tsMsgTx, sizeof(tsMsgRobotSrvHeader));
                                _tsMsgTx.header.txid++;
                                _stat.tx ++;
                                _stat.txbytes += sizeof(tsMsgRobotSrvHeader);
                                break;
                            default:
                                // throw std::string("not a compatible socket");
                                break;
                        }
                    }
                }

                
                /* */
                /* core of robot client feature   */
                /* state machine                  */
                switch(_state)
                {
                    case RC_INIT:
                        if (L_bSendDone==false)
                        {
                            _tsMsgTx.header.type  = ROBOT_INIT;
                            _tsMsgTx.header.state = (uint8_t) _state;
                            /* send packet */
                            cout << "send init msg" <<endl;
                            _stream->send((char *)&_tsMsgTx, sizeof(tsMsgRobotSrvHeader));
                            _stat.tx ++;
                            _stat.txbytes += sizeof(tsMsgRobotSrvHeader);
                            _tsMsgTx.header.txid++;
                            _tsMsgTx.header.rxid = 0;
                            L_bSendDone = true;
                        } 

                        break;
                    case RC_CONNECTED:
                        if (L_bSendDone==false)
                        {
                            _tsMsgTx.header.type  = ROBOT_CONNECT;
                            _tsMsgTx.header.state = (uint8_t) _state;
                            /* send packet */
                            cout << "send connected msg" <<endl;
                            _tsMsgTx.header.rxid = _tsMsgRx.header.txid;
                            _stream->send((char *)&_tsMsgTx, sizeof(tsMsgRobotSrvHeader));
                            _tsMsgTx.header.txid++;
                            _stat.tx ++;
                            _stat.txbytes += sizeof(tsMsgRobotSrvHeader);
                            L_bSendDone = true;
                        }
                        break;
                    case RC_RUNNING:
                        /* send status every 2048 packets/timeout ( 2 second)*/
                        if (0==(L_i32Idx%2048))
                        {
                            _tsMsgTx.header.type  = ROBOT_KEEPALIVE;
                            _tsMsgTx.header.state = (uint8_t) _state;
                            /* send packet */
                            _stream->send((char *)&_tsMsgTx, sizeof(tsMsgRobotSrvHeader));
                            _tsMsgTx.header.txid++;
                            _tsMsgTx.header.rxid = _tsMsgRx.header.txid;
                            _stat.tx ++;
                            _stat.txbytes += sizeof(tsMsgRobotSrvHeader);
                            L_bSendDone = true;
                        } 
                        L_i32Idx++;
                        break;
                    case RC_CALIBRATE_REQ:
                        /* wait until the calibration is ended */

                        /* TBD */
                        break;
                    case RC_STOPPED:
                        break;
                    default:
                        break;
                }


                // sleep(1);
                pthread_mutex_lock(&_mutex);
                L_bLoop = _bContinue==true && _bStop==true;
                pthread_mutex_unlock(&_mutex);                
            }
        } else {
            /* wait 10 seconds before to retry connection */
            cout << "RobotClientThreadCl : impossible to establish stream connection "<< _host << ':' << _port <<endl;
            sleep(10);
        }
        _state    = RC_STOPPED;
        pthread_mutex_lock(&_mutex);
        L_bLoop = _bStop;
        pthread_mutex_unlock(&_mutex);
        delete _stream;
        _stream =NULL;
        cout << "_stat.connected = " << _stat.connected << endl;
        cout << "_stat.retry     = " << _stat.retry     << endl;
        cout << "_stat.tx        = " << _stat.tx        << endl;
        cout << "_stat.txbytes   = " << _stat.txbytes   << endl;
        cout << "_stat.rx        = " << _stat.rx        << endl;
        cout << "_stat.rxbytes   = " << _stat.rxbytes   << endl;        
    }
    delete _connector;
    _connector=NULL;
    
    TRACES_INFO("end of core of RobotClientThreadCl");
    return 0;
}


RobotClientThreadCl::~RobotClientThreadCl()
{
    stop();

    /* wait until stream and _connector deleted */
    while ( _connector != NULL) sleep(1);

    std::cout << "~RobotClientThreadCl : delete done" << endl;
}


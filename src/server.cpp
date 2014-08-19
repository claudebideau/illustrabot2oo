/*!
 * \file server.cpp
 * \author claude.bideau@gmail.com
 * \version 0.1
 * \brief This module provides a server declaration.
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
#include <string>
#include <map>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "gpio.h"
#include "server.h"
#include "trace.h"
#include "ServerSocket.h"
#include "SocketException.h"
#include "proto.h"
#include <stdio.h>

using namespace std;


const std::string SERVER_KEYS[] =  {"SERVER_INIT", "SERVER_READY", "SERVER_CONNECTED", "SERVER_STOPPED"};

ServerThCl * E_pServerThObj= NULL;

ServerThCl::ServerThCl(iniCl * _pIni )
{
    std::string L_StrName;
    unsigned char L_u8Idx;
	std::string L_keys;
    std::list<std::string> L_listKey;
	std::list<std::string>::iterator L_it;
    std::string L_sInput = "";
	
    _state     = SERVER_INIT;

    TRACES_MSG("ServerThCl: init");

    /* create and associate Rt Trace object */
    _trace = new RtTrace("ServerThCl", RT_TRACE_ENABLE );
    _trace->trace(0, RT_TRACE_ENABLE, 0xFFFFFFFF, 2);
    
	L_listKey = _pIni->getItems("main");
	if (!L_listKey.empty())
	{
        _sockId = (int) _pIni->getInteger("main","server",-1 );
        if (_sockId == -1)
        {
            TRACES_WARNING("WARNING the socket port has not been set in ini file (section main / field server)!!!");
            while (true) 
            {
                std::cout << "\nPlease enter the socket port [9099 by default]:";
                getline(cin, L_sInput);

               // This code converts from string to number safely.
               if (L_sInput.size() == 0 ) { _sockId = 9099 ; break; }
               stringstream myStream(L_sInput);
               if (myStream >> _sockId) break;
               cout << "Invalid number, please try again" << endl;
            }
        }
    
    }
    _state     = SERVER_INIT;
    _bContinue = false;

	
}

/**
   @brief wakeup the stepper driver
   @return none.
 */
teServerState ServerThCl::state(void)
{
    return _state;
}


void ServerThCl::stop(void)
{ 
    _bContinue = false;
    return;
}



void *ServerThCl::_execute(void)
{
    int L_i32Idx = 0;

    _bContinue = true;
    
    TRACES_INFO_ARG1("execute core of ServerThCl on port %d",_sockId );
    try
    {

        _pServer = new ServerSocket( _sockId );
        _state = SERVER_RUNNING;
        while (_bContinue==true)
        {

            ServerSocket new_sock;
            _pServer->accept ( new_sock );
            std::cout << "accept"<<std::endl;

            try
            {
                while ( true )
                {
                    message_t data;
                    std::string answer("OK\n");

                    new_sock.recv((void*)&data);
                    hex_dump_message((void*)&data);
                    //new_sock >> data;
                    new_sock << answer;
                }
            }
            catch ( SocketException& ) {}

        }
    }
    catch ( SocketException& e )
    {
        std::cout << "Exception was caught:" << e.description() << "\nExiting.\n";
    }

    _state = SERVER_STOPPED;
    TRACES_INFO("end of core of ServerThCl");
    return 0;
}


ServerThCl::~ServerThCl()
{
    _bContinue = false;

    std::cout << "~ServerThCl : delete done" << endl;
}


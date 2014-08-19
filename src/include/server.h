/*!
 * \file server.h
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
#ifndef SERVER_H
#define SERVER_H

/**   1. Standard Library Include                                   **/
#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <queue>


/**   1. Include files  (own)                                       **/
#include "arm.h"
#include "inireader.h"
#include "proto.h"
#include "ServerSocket.h"
#include "SocketException.h"
#include "proto.h"

/**   2a.   External Functions                                      **/
/**   2b.   External Data                                           **/
/**   3.  Type Definitions                                          **/
/**   5.  Local Functions                                           **/
/**   6.  Macros / Defines                                          **/


typedef enum eServerState {SERVER_INIT=0, SERVER_READY, SERVER_RUNNING, SERVER_STOPPED   } teServerState;


 /**
 * \class ServerThCl
 * \brief 
 *
 */
class ServerThCl
{
    public:
		ServerThCl(iniCl * );

        teServerState state(void);

        static void *run(void *context)
        {
            return ((ServerThCl *)context)->_execute();
        }

        void stop(void);

        virtual ~ServerThCl();


    protected:
    private:
        bool               _bContinue;
        teServerState      _state;
        RtTrace          * _trace;
        int                _sockId;
        ServerSocket     * _pServer;


        void *_execute(void);        
};

extern const std::string SERVER_KEYS[];
extern ServerThCl * E_pServerThObj;


#endif // SERVER_H

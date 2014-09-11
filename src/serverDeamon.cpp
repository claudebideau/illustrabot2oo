/*!
 * \file debug_thread.cpp
 * \author claude.bideau@gmail.com
 * \version 0.1
 * \brief This module provides a easydrv declaration.
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
 /**   1. Standard Library Include                                   **/

#include <cassert>
#include <stdexcept>
#include <iostream>
#include <unistd.h>
#include <map>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include <string>


/**   1. Include files  (own)                                       **/
#include "proto.h"
#include "IssueException.h"
#include "inireader.h"
#include "trace.h"
#include "tcpacceptor.h"
#include "RobotSrvTh.h"
#include "UeSrvTh.h"
#include "DebugSrvTh.h"


using namespace std;

/**   2a.   External Functions                                      **/
/**   2b.   External Data                                           **/
/**   3.  Type Definitions                                          **/
typedef struct sSrvParam {
    int robot;
    int ue;
    int debug;

} tsSrvParam;

/**   5.  Local Functions                                           **/
/**   6.  Macros / Defines                                          **/


tsSrvParam  G_tsSrvParam;

/*---------------------------------------------------------*/
void DisplayHelp(char * F_pcname)
{
	cout << "Usage:" << endl << "\t" << F_pcname << "[-h] [-i <ini>]"<<endl;
	cout << endl;
	return;
}

/** Display a received message from the client as hex bytes
 * @param msg the message to dump
 */
//void hex_dump_message (std::string &msg)
void hex_dump_message (void * msg)
{
//   char* ptr = (char*)msg.c_str();
   char* ptr = (char*)msg;
   int i;

   for (i=0;i < sizeof(message_t);i++) {
      printf ("%02X ",(uint8_t) ptr[i]);
   }
   printf("\n");
}// hex_dump_message

/**
 * @brief server deamon
 * @param F_pIni ini object
 * @param argv arguments
 * @return error status.
 */
int Q_i32GetIniParam(iniCl * F_pIni, tsSrvParam *  F_ptsSrvParam)
{
    int L_err = -1;
    std::list<std::string> L_listKey;
    
    
	L_listKey = F_pIni->getItems("server");
	if (!L_listKey.empty())
	{
        F_ptsSrvParam->robot  = F_pIni->getInteger("server","robot", -1 );
        F_ptsSrvParam->ue     = F_pIni->getInteger("server","ue"   , -1 );
        F_ptsSrvParam->debug  = F_pIni->getInteger("server","debug", -1 );
        if (F_ptsSrvParam->robot == -1)
        {
            throw std::string("The socket port has not been set in ini file (section server / field robot)!!!");
        }
        if (F_ptsSrvParam->ue == -1)
        {
            throw std::string("The socket port has not been set in ini file (section server / field ue)!!!");
        }
    } else throw  std::string("can't find section [server] ini file:\n[server]\nrobot=<port>\nue=<port>\n optional:\ndebug=<port>\n") ;
    return 0;
}

/**
 * @brief server deamon
 * @param argc number of argument
 * @param argv arguments
 * @return none.
 */
int main ( int argc, char * argv[] )
{
    iniCl *cini;
	char *inifile = NULL;
	int L_i32option;

    TCPStream* stream = NULL;
    TCPAcceptor* acceptor = NULL;
    pthread_t L_RobotSrvTh;
    pthread_t L_UeSrvTh;
    pthread_t L_DebugSrvTh;
    
    RobotSrvThreadCl *   L_ptsRobotSrvThreadObj = NULL;
    UeSrvThreadCl *       L_ptsUeSrvThreadObj = NULL;
    DebugSrvThreadCl * L_ptsDebugSrvThreadObj = NULL;

    TRACES_INFO(__FILE__"main function");
    TRACES_INFO_ARG1("Compiler version %d",__cplusplus);

    memset(&G_tsSrvParam,0,sizeof(tsSrvParam));
    
    /* ------------------------------------------------------------------ */
    /*  - socket management:                                              */
    /*     2 ports:                                                       */
    /*       - robot port for communication with the robot                */
    /*       - UE port for communication with one or 2 UEs                */
    /*    => require 1 thread for robot port                              */
    /*    => require 1 or 2 threads with UE                               */
    /* ------------------------------------------------------------------ */
    try
    {
        while ((L_i32option = getopt (argc, argv, "hi:")) != -1)
        {
		    switch (L_i32option)
            {
                case 'h':
                    DisplayHelp(argv[0]);
                    exit(0);
				    break;
				case 'i':
                    inifile = optarg;
                    break;
                case '?':
                    throw std::string("Unknown option character");
                    return 1;
                default:
                    abort ();
            }
        }
        if (inifile == NULL) throw std::string("You must specify ini file");
        cini = new iniCl(inifile);
        
        if (Q_i32GetIniParam(cini,&G_tsSrvParam)!=0) throw std::string("Impossible to get server parameters");
    }
    catch(std::string &error)
    {
        cerr<<"Error: "<<error<<endl;
        return -1;
    }
    catch(char* error)
    {
        cerr<<"Error: "<<error<<endl;
        return -1;
    } 

    bool L_bStop = false;
    /*main loop */
    while (!L_bStop)
    {
        try
       {
            // Create the socket Robot 2 Srv
            // Create the socket Ue 2 Srv

            L_ptsRobotSrvThreadObj = new RobotSrvThreadCl(G_tsSrvParam.robot);
            pthread_create(&L_RobotSrvTh, NULL, &RobotSrvThreadCl::run, L_ptsRobotSrvThreadObj);

            L_ptsUeSrvThreadObj = new UeSrvThreadCl(G_tsSrvParam.ue, L_ptsRobotSrvThreadObj);
            pthread_create(&L_UeSrvTh, NULL, &UeSrvThreadCl::run, L_ptsUeSrvThreadObj);

            L_ptsDebugSrvThreadObj = new DebugSrvThreadCl(G_tsSrvParam.debug, L_ptsRobotSrvThreadObj, L_ptsUeSrvThreadObj);
            pthread_create(&L_DebugSrvTh, NULL, &DebugSrvThreadCl::run, L_ptsDebugSrvThreadObj);

            pthread_join(L_RobotSrvTh,NULL);
            pthread_join(L_UeSrvTh,NULL);        
            pthread_join(L_DebugSrvTh,NULL);

        }
        catch(IssueCl const &error)
        {
            cerr << "Error:" << error.what() << endl;
            cerr << "    service = " << error.getService() <<endl;
        }
        catch(std::string &error)
        {
            cerr<<"Error: "<<error<<endl;
            L_bStop = true;
        }
        catch(char* error)
        {
            cerr<<"Error: "<<error<<endl;
            L_bStop = true;
        } 
    }
    return 0;
}



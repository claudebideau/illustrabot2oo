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
#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/registry.hpp>
#include <xmlrpc-c/server_abyss.hpp>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <queue>

/**   1. Include files  (own)                                       **/
#include "rpc/xmlrpc_method.h"
#include "inireader.h"
#include "ms_element.h"
#include "trace.h"
#include "angle.h"
#include "orientation.h"
#include "RobotClientTh.h"
// #include "server.h"
// #include "clientTh.h"

using namespace std;


RobotClientThreadCl * G_pClientThObj=NULL;


/*---------------------------------------------------------*/
class RpcShutdownCl : public xmlrpc_c::registry::shutdown {
   public:
       RpcShutdownCl(xmlrpc_c::serverAbyss * const serverHandle) : serverHandle(serverHandle) {}

       void doit(string const& comment,void * const) const 
       {
            cerr << "Shutting down because " << comment <<endl;
            if (G_pClientThObj != NULL)
                G_pClientThObj->stop();
            if (E_pOrientationThObj != NULL)
                E_pOrientationThObj->stop();
            sleep(1);
            delete G_pClientThObj;
            G_pClientThObj = NULL;
            delete E_pOrientationThObj;
            E_pOrientationThObj = NULL;
            cerr << "terminate " << comment <<endl;
            serverHandle->terminate();
       }

   private:
       xmlrpc_c::serverAbyss * const serverHandle;
};

/*---------------------------------------------------------*/
void DisplayHelp(char * F_pcname)
{
	cout << "Usage:" << endl << "\t" << F_pcname << "[-h] [-i <ini>]"<<endl;
	cout << endl;
	return;
}

// ArmCl * E_pArmObj = NULL;

int main(int argc, char **  argv) 
{
    iniCl *cini;
	char *inifile = NULL;
	int L_i32option;
    pthread_t L_OrientationTh;
    pthread_t L_ClientTh;

    TRACES_INFO(__FILE__"main function");
    TRACES_INFO_ARG1("Compiler version %d",__cplusplus);
    
    Q_vInitAlphaTable();

	opterr = 0;
     
    try {
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

        xmlrpc_c::registry L_rpcRegistry;

        cini = new iniCl(inifile);
        E_pOrientationThObj = new OrientationThCl(cini );
        // E_pServerThObj     = new ServerThCl(cini );

        G_pClientThObj     = new RobotClientThreadCl(cini);
        
        TRACES_INFO("Create pthread for orientation");
        pthread_create(&L_OrientationTh, NULL, &OrientationThCl::run, E_pOrientationThObj);        

        TRACES_INFO("Create pthread for client TH");
        pthread_create(&L_ClientTh, NULL, &RobotClientThreadCl::run, G_pClientThObj);     
        
        // TRACES_INFO("==========  CHECK INI  ==========");
		// for (L_itElement = G_MapElementObj.begin() ; L_itElement != G_MapElementObj.end(); ++L_itElement)
        // {
            // cout << *(L_itElement->second) << endl;
        // }
        // TRACES_INFO("==========  EOCHECK INI  ==========");

        TRACES_INFO("Prepare XML-RPC attachment");

        TraceRpcAttach(&L_rpcRegistry );
        RtTraceRpcAttach(&L_rpcRegistry ); 
        ElementRpcAttach(&L_rpcRegistry );
        osAttach(&L_rpcRegistry );
        ArmRpcAttach(&L_rpcRegistry );
        OrientationRpcAttach(&L_rpcRegistry);

        

        xmlrpc_c::serverAbyss L_rpcAbyssServer(
            xmlrpc_c::serverAbyss::constrOpt()
            .registryP(&L_rpcRegistry)
            .portNumber(8000));

        RpcShutdownCl shutdown(&L_rpcAbyssServer);
        // xmlrpc_c::serverAbyss::shutdown shutdown(&L_rpcAbyssServer);
        L_rpcRegistry.setShutdown(&shutdown);
        
        TRACES_INFO("run Rpc server");
        L_rpcAbyssServer.run();
  
    } catch (exception const& e) {
        // cerr << "Something failed.  " << e.what() << endl;
        std::string error(e.what());
        TRACES_ERROR_ARG1("Something failed. %s ", error.c_str());
    }
    catch(std::string &error)
    {
        cerr<<"Error: "<<error<<endl;
    }
    catch(char* error)
    {
        cerr<<"Error: "<<error<<endl;
    }

    return 0;
}

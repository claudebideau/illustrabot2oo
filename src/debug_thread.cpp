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

/**   1. Include files  (own)                                       **/
#include "rpc/xmlrpc_method.h"
#include "inireader.h"
#include "ms_element.h"
#include "arm.h"
#include "trace.h"

using namespace std;


void DisplayHelp(void)
{
	cout << "Usage:" << endl;
	cout << "\tdebug_thread [-h] [-i <ini>]"<<endl;
	cout << endl;
	return;
}

extern void Q_vInitCheckLattitudeTable(short F_i16Heigh);
extern void Q_vInitAlphaTable(void);

// ArmCl * E_pArmObj = NULL;


int main(int argc, char **  argv) 
{
    iniCl *cini;
	char *inifile = NULL;
	int index;
	int c;
	std::list<std::string> L_listSection;
	std::string  L_element;
	std::string  L_type;
    std::string  L_MS_name;

    
	std::vector<std::string> tokens;
	std::vector<std::string>::iterator L_it ;
	std::map<std::string, MotorSensorElementCl *>::iterator L_itElement ;
    
    TRACES_INFO(__FILE__"main function");
    TRACES_INFO_ARG1("Compiler version %d",__cplusplus);

    
    Q_vInitAlphaTable();
    Q_vInitCheckLattitudeTable(1024);


	opterr = 0;
     
    try {
        while ((c = getopt (argc, argv, "hi:")) != -1)
        {
		    switch (c)
            {
                case 'h':
                    DisplayHelp();
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

        xmlrpc_c::registry myRegistry;

        cini = new iniCl(inifile);
		L_listSection = cini->sections();
#if 0
		L_element = cini->get("main","element");
		tokens = split(L_element," ",true);

		// std::cout << "tokens contains:" << endl;
		for (L_it = tokens.begin() ; L_it != tokens.end(); ++L_it)
		{
            L_MS_name = *L_it;
            std::cout << " '" << L_MS_name << "'" <<endl;
            
			L_type = cini->get(*L_it,"type");
			// std::cout << " type = " << L_type <<endl;
			if (strcmp(L_type.c_str(),"single")==0)
			{
				G_MapElementObj[L_MS_name] = new MotorSensorElementCl(cini, L_MS_name);
			} else {
				// cout << "type '" << L_type << "' not implemented" << endl;
                TRACES_WARNING_ARG1("Unknown type %s ", L_type.c_str());
			}
		}
#else
        E_pArmObj = new ArmCl(cini,"arm");
#endif
        
        TRACES_INFO("==========  CHECK INI  ==========");
#if 0
		for (L_itElement = G_MapElementObj.begin() ; L_itElement != G_MapElementObj.end(); ++L_itElement)
        {
            cout << *(L_itElement->second) << endl;
        }
#else
        cout << *E_pArmObj <<endl;
#endif
        TRACES_INFO("==========  EOCHECK INI  ==========");


        TraceRpcAttach(&myRegistry );
        RtTraceRpcAttach(&myRegistry ); 
        ElementRpcAttach(&myRegistry );
        osAttach(&myRegistry );
        ArmRpcAttach(&myRegistry );

        
        xmlrpc_c::serverAbyss myAbyssServer(
            xmlrpc_c::serverAbyss::constrOpt()
            .registryP(&myRegistry)
            .portNumber(8000));

        xmlrpc_c::serverAbyss::shutdown shutdown(&myAbyssServer);
        myRegistry.setShutdown(&shutdown);
        
        myAbyssServer.run();
  
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

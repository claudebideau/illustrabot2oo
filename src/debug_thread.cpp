/*!
 * \file easydrv.hpp
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

/**   1. Include files  (own)                                       **/
#include "rpc/xmlrpc_method.h"
#include "inireader.h"
#include "ms_element.h"
#include "trace.h"

using namespace std;



int 
main(int           const, 
     const char ** const) {
    iniCl *cini;
    
	std::list<std::string> L_listSection;
	std::string  L_element;
	std::string  L_type;
	std::vector<std::string> tokens;
	std::vector<std::string>::iterator L_it ;

	std::map<std::string, MotorSensorElementCl *>::iterator L_itElement ;
    
    TRACES_INFO("xmlrpc_thread main function");
    try {
        xmlrpc_c::registry myRegistry;
        cini = new iniCl("tst.ini");
		L_listSection = cini->sections();
		L_element = cini->get("main","element");
		tokens = split(L_element," ",true);

		// std::cout << "tokens contains:" << endl;
		for (L_it = tokens.begin() ; L_it != tokens.end(); ++L_it)
		{
			// std::cout << " '" << *L_it << "'";
			L_type = cini->get(*L_it,"type");
			// std::cout << " type = " << L_type <<endl;
			if (strcmp(L_type.c_str(),"single")==0)
			{
				G_MapElementObj[*L_it] = new MotorSensorElementCl(*cini, *L_it);
			} else {
				// cout << "type '" << L_type << "' not implemented" << endl;
                TRACES_WARNING_ARG1("Unknown type %s ", L_type.c_str());
			}
		}
        
        TRACES_INFO("==========  CHECK INI  ==========");
		for (L_itElement = G_MapElementObj.begin() ; L_itElement != G_MapElementObj.end(); ++L_itElement)
        {
            cout << *(L_itElement->second) << endl;
        }
        TRACES_INFO("==========  EOCHECK INI  ==========");


        TraceRpcAttach(&myRegistry );
        RtTraceRpcAttach(&myRegistry ); 
        ElementRpcAttach(&myRegistry );
        
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

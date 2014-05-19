/*!
 * \file calibrate.hpp
 * \author claude.bideau@gmail.com
 * \version 0.1
 * \brief This module provides a calibrate declaration.
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
#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/registry.hpp>
#include <xmlrpc-c/server_abyss.hpp>

/**   1. Include files  (own)                                       **/
#include "rpc/xmlrpc_method.h"

using namespace std;



CalibrateMethodCl::CalibrateMethodCl()
{
        // signature and help strings are documentation -- the client
        // can query this information with a system.methodSignature and
        // system.methodHelp RPC.
        this->_signature = "s:i";
            // method's result and two arguments are integers
        this->_help = "this method offer the functionality to calibrate an element.\n\
options:\n\
\t<name>  : name of the element you want to calibrate\n\
\nif <name> not specify provide the list of element and the current state\n\
";
    return;
}
    
void CalibrateMethodCl::execute(xmlrpc_c::paramList const& paramList,xmlrpc_c::value *   const  retvalP) 
{
        
        
        if (paramList.size() == 0)
        {
            /* return list of element */
            *retvalP = xmlrpc_c::value_string("empty");
        } else {
            std::string const name(paramList.getString(0));
        
            *retvalP = xmlrpc_c::value_int(0);

        }
}

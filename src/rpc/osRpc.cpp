/*!
 * \file osRcp.cpp
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
#include <sstream>
#include <unistd.h>
#include <list>
#include <map>
#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/registry.hpp>
#include <xmlrpc-c/server_abyss.hpp>

/**   1. Include files  (own)                                       **/
#include "rpc/xmlrpc_method.h"
#include "trace.h"

using namespace std;


 /**
 * \class osInitCl
 * \brief 
 *
 */
class osInitCl : public xmlrpc_c::method
{
    public:
        osInitCl()
        {
                this->_signature = "s:i";
                this->_help = "\n\
usage : xml.os.init [<init value>] \n\n\
\tset new level value\n\
\n\n parameters:\n\
\t<init value> : init level \n\
\nreturn :\n\
\t<status> \n\
\t\n\n";
            return;
        }
            
        void execute(xmlrpc_c::paramList const& paramList,xmlrpc_c::value *   const  retvalP) 
        {
            switch(paramList.size())
            {
                case 1:
                    {
                        int const L_initValue(paramList.getInt(0));
                        char cmd[32];
                        int L_i32res;
                        snprintf(cmd,1024, "init %d",L_initValue);
                        L_i32res= system(cmd);
                        *retvalP = xmlrpc_c::value_int(L_i32res);
                    }
                    break;

                default:
                    *retvalP = xmlrpc_c::value_int(-1);
                    throw "require only 1 parameters <init value>";
                    break;
            }
            return;
        }
        

};

void osAttach(xmlrpc_c::registry * F_pRegistry)
{

    xmlrpc_c::methodPtr const osInitObj(new osInitCl);

    F_pRegistry->addMethod("xml.os.init",       osInitObj);

    return ;
}

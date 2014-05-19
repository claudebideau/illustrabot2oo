/*!
 * \file rttracercp.cpp
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


// template<typename keyType, typename valType> class myMap
// : public std::map<keyType, valType> {

// public:
// myMap<keyType, valType> &operator()(keyType k, valType v)
// {
// (*this)[k]=v;
// return *this;
// };
// };

static std::map<std::string, int> G_mParamMask;


 /**
 * \class TraceSetLevelCl
 * \brief 
 *
 */
class TraceSetLevelCl : public xmlrpc_c::method
{
    public:
        TraceSetLevelCl()
        {
                this->_signature = "s:i";
                this->_help = "\n\
usage : trace.level.set <fieldname> <value>\n\n\
\tset new level value\n\
\n\n parameters:\n\
\t<fieldname> : ERROR / WARNING / CMD / MSG / LOAD / INFO / DEBUG \n\
\t<value>     : 0 disable / 1 : enable trace level\n\
\nreturn :\n\
\t<newlevel>: \n\
\t\n\n";
            return;
        }
            
        void execute(xmlrpc_c::paramList const& paramList,xmlrpc_c::value *   const  retvalP) 
        {
            // std::list<std::string> list_trace(G_RtTraceMapObj.list());

            switch(paramList.size())
            {
                case 2:
                    {
                        // RtTrace * L_RtTraceBuffer;
                        std::map<std::string,int>::iterator it;
                        unsigned short L_level;
                        std::string const L_name(paramList.getString(0));
                        xmlrpc_c::value L_value = paramList[1];
                        // cout << L_value.type() << L_value.cValue().cvalue() << endl;
                        switch (L_value.type())
                        {
                            case xmlrpc_c::value::TYPE_STRING :
                                    {
                                        std::string  L_levelStr(paramList.getString(1));
                                        istringstream ( L_levelStr ) >> L_level;
                                    }
                                    break;
                            case xmlrpc_c::value::TYPE_INT:
                                    {
                                        L_level = paramList.getInt(1);
                                    }
                                    break;
                            default :
                                *retvalP = xmlrpc_c::value_string("ISSUE : require only two parameter <trace case> <level>");
                                return;
                                break;
                        }
                        
                        it = G_mParamMask.find(L_name);
                        if (it != G_mParamMask.end())
                        {
                            unsigned int L_u32MaskValue = G_mParamMask[L_name];
                            if (L_level == 0) E_u32TraceLevel &= (~L_u32MaskValue);
                            if (L_level == 1) E_u32TraceLevel |= (L_u32MaskValue);
                            
                        }
                        *retvalP = xmlrpc_c::value_int(0);
                    }
                    break;
                default:
                    *retvalP = xmlrpc_c::value_string("ISSUE : require only two parameter <trace case> <level>");
                    break;
            }
            return;    
        }

};


 /**
 * \class TraceGetLevelCl
 * \brief 
 *
 */
class TraceGetLevelCl : public xmlrpc_c::method
{
    public:
        TraceGetLevelCl()
        {
                this->_signature = "n:i";
                this->_help = "\n\
usage : trace.level.get \n\n\
\tprovide the current bitmask value level value\n\
\n\n no parameters:\n\
\nreturn :\n\
\t<trace level>: \n\
\t\n\n";
            return;
        }
            
        void execute(xmlrpc_c::paramList const& paramList,xmlrpc_c::value *   const  retvalP) 
        {
            
            std::map<std::string, xmlrpc_c::value> L_structData;
            std::map<std::string, int>::iterator it;
            pair<string, xmlrpc_c::value> member;
            
            for (it =G_mParamMask.begin(); it != G_mParamMask.end(); ++it)
            {
                unsigned int L_u32mask = E_u32TraceLevel & (it->second);
                
                std::string L_maskName = it->first;
                xmlrpc_c::value_int L_value(L_u32mask != 0);
                pair<string, xmlrpc_c::value> member(L_maskName, L_value);
                L_structData.insert(member); 
            }
                
            *retvalP =  xmlrpc_c::value_struct(L_structData);

            return;
        }
};

void TraceRpcAttach(xmlrpc_c::registry * F_pRegistry)
{
    G_mParamMask["ERROR_LEVEL"]=   TRACES_LEVEL_ERROR   ;
    G_mParamMask["WARNING_LEVEL"]= TRACES_LEVEL_WARNING ;
    G_mParamMask["CMD_LEVEL"]=     TRACES_LEVEL_CMD     ;
    G_mParamMask["MSG_LEVEL"]=     TRACES_LEVEL_MSG     ;
    G_mParamMask["LOAD_LEVEL"]=    TRACES_LEVEL_LOAD    ;
    G_mParamMask["INFO_LEVEL"]=    TRACES_LEVEL_INFO    ;
    G_mParamMask["DEBUG_LEVEL"]=   TRACES_LEVEL_DEBUG   ;
                                                              
    xmlrpc_c::methodPtr const TraceSetLevelObj(new TraceSetLevelCl);
    xmlrpc_c::methodPtr const TraceGetLevelObj(new TraceGetLevelCl);

    F_pRegistry->addMethod("xml.trace.level.set",       TraceSetLevelObj);
    F_pRegistry->addMethod("xml.trace.level.get",       TraceGetLevelObj);

    return ;
}

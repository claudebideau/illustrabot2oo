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
#include <unistd.h>
#include <list>
#include <map>
#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/registry.hpp>
#include <xmlrpc-c/server_abyss.hpp>

/**   1. Include files  (own)                                       **/
#include "rpc/xmlrpc_method.h"
#include "rttrace.h"

using namespace std;



/**
 * \class RtTraceInfoCl
 * \brief 
 *
 */
class RtTraceInfoCl : public xmlrpc_c::method
{
    public:

        RtTraceInfoCl()
        {
            this->_signature = "s:A";
            this->_help = "dump rttrace info";
            return;
        }
            
        void execute(xmlrpc_c::paramList const& paramList,xmlrpc_c::value *   const  retvalP) 
        {
            std::list<std::string> list_trace(G_RtTraceMapObj.list());
            vector<xmlrpc_c::value> arrayData;
        
            for (std::list<std::string>::iterator it=list_trace.begin(); it != list_trace.end(); ++it)
            {
                arrayData.push_back(xmlrpc_c::value_string(*it));
            }

            xmlrpc_c::value_array array1(arrayData);
            *retvalP = array1;


            return;  
        }
};

 /**
 * \class RtTraceDumpCl
 * \brief 
 *
 */
class RtTraceDumpCl : public xmlrpc_c::method
{
    public:

        RtTraceDumpCl()
        {
                this->_signature = "s:A";
                this->_help = "dump rttrace level";
            return;
        }
            
        void execute(xmlrpc_c::paramList const& paramList,xmlrpc_c::value *   const  retvalP) 
        {
            std::list<std::string> list_trace(G_RtTraceMapObj.list());

            switch(paramList.size())
            {
                case 1:
                    {
                        RtTrace * L_RtTraceBuffer;
                        std::string const L_name(paramList.getString(0));
                        
                        L_RtTraceBuffer = G_RtTraceMapObj.get(L_name);
                        if (L_RtTraceBuffer != NULL)
                        {
                            unsigned int   L_u32aBuffer[MAX_BUFFER_SIZE];
                            unsigned int   L_u32Index;
                            map<string, xmlrpc_c::value> L_structData;
   
                            L_u32Index = L_RtTraceBuffer->getTrace(L_u32aBuffer);
                            pair<string, xmlrpc_c::value> member("ptr", xmlrpc_c::value_int(L_u32Index));
                            L_structData.insert(member);

                            vector<xmlrpc_c::value> L_arrayData;
                        
                            for (unsigned int L_u32Idx=0;L_u32Idx< MAX_BUFFER_SIZE; L_u32Idx++)
                            {
                                L_arrayData.push_back(xmlrpc_c::value_int(L_u32aBuffer[L_u32Idx]));
                            }
                            pair<string, xmlrpc_c::value> member2("data", xmlrpc_c::value_array(L_arrayData));
                            L_structData.insert(member2);

                            pair<string, xmlrpc_c::value> member3("buffer", xmlrpc_c::value_string(L_name));
                            L_structData.insert(member3);

                            pair<string, xmlrpc_c::value> member4("level", xmlrpc_c::value_int(L_RtTraceBuffer->getLevel()));
                            L_structData.insert(member4);

                            pair<string, xmlrpc_c::value> member5("format", xmlrpc_c::value_string(L_RtTraceBuffer->format));
                            L_structData.insert(member5);
                            
                            // xmlrpc_c::value_array L_data(L_arrayData);
                            // cout << "cnv1" << endl;
                            // vector<unsigned char> const L_data(&L_u32aBuffer[0], &L_u32aBuffer[sizeof(L_u32aBuffer)]);

                            // cout << "cnv1" << endl;
                            // xmlrpc_c::value_bytestring const L_base64(L_data);
                            // cout << "cnv2" << endl;

                            *retvalP =  xmlrpc_c::value_struct(L_structData);
                        } else {
                            *retvalP = xmlrpc_c::value_int(-1);
                        }
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
 * \class RtTraceSetLevelCl
 * \brief 
 *
 */
class RtTraceSetLevelCl : public xmlrpc_c::method
{
    public:
        RtTraceSetLevelCl()
        {
                this->_signature = "s:i,i:i";
                this->_help = "set rttrace level";
            return;
        }
            
        void execute(xmlrpc_c::paramList const& paramList,xmlrpc_c::value *   const  retvalP) 
        {
            std::list<std::string> list_trace(G_RtTraceMapObj.list());

            switch(paramList.size())
            {
                case 2:
                    {
                        RtTrace * L_RtTraceBuffer;
                        std::string const L_name(paramList.getString(0));
                        unsigned short const L_level(paramList.getInt(1));
                        
                        L_RtTraceBuffer = G_RtTraceMapObj.get(L_name);
                        if (L_RtTraceBuffer != NULL)
                        {
                            L_RtTraceBuffer->setLevel(L_level);
                            *retvalP = xmlrpc_c::value_int(0);
                        } else {
                            *retvalP = xmlrpc_c::value_int(-1);
                        }
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
 * \class RtTraceGetLevelCl
 * \brief 
 *
 */
class RtTraceGetLevelCl : public xmlrpc_c::method
{
    public:
        RtTraceGetLevelCl()
        {
                this->_signature = "n:i";
                this->_help = "get rttrace level";
            return;
        }
            
        void execute(xmlrpc_c::paramList const& paramList,xmlrpc_c::value *   const  retvalP) 
        {
            std::list<std::string> list_trace(G_RtTraceMapObj.list());

            switch(paramList.size())
            {
                case 0:
                    {
                        vector<xmlrpc_c::value> arrayData;
                    
                        for (std::list<std::string>::iterator it=list_trace.begin(); it != list_trace.end(); ++it)
                        {
                            arrayData.push_back(xmlrpc_c::value_string(*it));
                        }

                        xmlrpc_c::value_array array1(arrayData);
                        *retvalP = array1;
                    }
                    break;
                case 1:
                    {
                        RtTrace * L_RtTraceBuffer;
                        std::string const L_name(paramList.getString(0));
                        
                        L_RtTraceBuffer = G_RtTraceMapObj.get(L_name);
                        if (L_RtTraceBuffer != NULL)
                        {
                            *retvalP = xmlrpc_c::value_int(L_RtTraceBuffer->getLevel());
                        } else {
                            *retvalP = xmlrpc_c::value_int(-1);
                        }
                    }
                    break;
                default:
                    *retvalP = xmlrpc_c::value_int(-2);
                    throw "require only one parameter";
                    break;
            }
            return;
        }
};

void RtTraceRpcAttach(xmlrpc_c::registry * F_pRegistry)
{
    xmlrpc_c::methodPtr const RtTraceInfoObj(new RtTraceInfoCl);
    xmlrpc_c::methodPtr const RtTraceSetLevelObj(new RtTraceSetLevelCl);
    xmlrpc_c::methodPtr const RtTraceGetLevelObj(new RtTraceGetLevelCl);
    xmlrpc_c::methodPtr const RtTraceDumpObj    (new RtTraceDumpCl);

    F_pRegistry->addMethod("xml.rttrace.info",      RtTraceInfoObj);
    F_pRegistry->addMethod("xml.rttrace.level.set", RtTraceSetLevelObj);
    F_pRegistry->addMethod("xml.rttrace.level.get", RtTraceGetLevelObj);
    F_pRegistry->addMethod("xml.rttrace.dump",      RtTraceDumpObj    );

    return ;
}

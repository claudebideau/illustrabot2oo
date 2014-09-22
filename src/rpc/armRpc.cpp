/*!
 * \file armRcp.cpp
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
#include "arm.h"
#include "rttrace.h"
#include "trace.h"

using namespace std;


 /**
 * \class ArmInfoCl
 * \brief 
 *
 */
class ArmInfoCl : public xmlrpc_c::method
{
    public:
        ArmInfoCl()
        {
            this->_signature = "n:A";
            this->_help = "\n\
usage : xml.arm.info [<name>] \n\n\
\treturn the list of existing arm or information if arm <name> is filled \n\
\n\n parameters:\n\
\t<name>      : name of the arm to get the information\n\
\nreturn :\n\
\t<list of arm name> \n\
\t<struct>  : struct of data for <name> arm \n\
\t\n\n";
            return;
        };
        
        void execute(xmlrpc_c::paramList const& paramList,xmlrpc_c::value *   const  retvalP) 
        {

            switch(paramList.size())
            {
                case 0:
                    {
                        // RtTrace * L_RtTraceBuffer;
                        map<string, xmlrpc_c::value> L_structData;
                        tsArmPosition L_tsArmPosition;

                        if (E_pArmObj != NULL)
                        {
                            pair<string, xmlrpc_c::value> member_name("name", xmlrpc_c::value_string(E_pArmObj->name()));
                            pair<string, xmlrpc_c::value> member_state("state", xmlrpc_c::value_int(E_pArmObj->state()));
                            E_pArmObj->get(&L_tsArmPosition);
                            pair<string, xmlrpc_c::value> member_longitude("longitude", xmlrpc_c::value_int(L_tsArmPosition.longitude));
                            pair<string, xmlrpc_c::value> member_latitude("latitude", xmlrpc_c::value_int(L_tsArmPosition.latitude));
                            pair<string, xmlrpc_c::value> member_radius("radius", xmlrpc_c::value_int(L_tsArmPosition.radius));
                            L_structData.insert(member_name);
                            L_structData.insert(member_state);
                            L_structData.insert(member_longitude);
                            L_structData.insert(member_latitude);
                            L_structData.insert(member_radius);
        
                            *retvalP =  xmlrpc_c::value_struct(L_structData);

                        } else {
                            *retvalP = xmlrpc_c::value_int(-1);
                        }
                    }
                    break;
                default:
                    *retvalP = xmlrpc_c::value_int(-2);
                    throw "require only no parameter";
                    break;
            }
            return;
        }
        
};


///**
// * \class ArmCalibrateCl
// * \brief 
// *
// */
//class ArmCalibrateCl : public xmlrpc_c::method
//{
//    public:
//        ArmCalibrateCl()
//        {
//            // this->_signature = "n:A";
//            this->_help = "\n\
//usage : xml.arm.calibrate <start/stop>\n\n\
//\tstart or stop calibration for arm \n\
//\n\n parameters:\n\
//\t<start/stop> : \n\
//\nreturn :\n\
//\t<status> : 0 done / otherwise error\n\
//\t\n\n";
//            return;
//        };
//        
//        void execute(xmlrpc_c::paramList const& paramList,xmlrpc_c::value *   const  retvalP) 
//        {
//            switch(paramList.size())
//            {
//
//                case 1:
//                    {
//                        // RtTrace * L_RtTraceBuffer;
//                        std::string const L_type(paramList.getString(1));
//                        int L_start=0;
//                        L_start= (L_type.compare("start") == 0);
//                        
//                        map<string, xmlrpc_c::value> L_structData;
//                        
//                        if (E_pArmObj != NULL)
//                        {
//                            if (L_start==TRUE)
//                            {
//                                TRACES_WARNING_ARG1("Calibration start request for '%s'",E_pArmObj->name().c_str());
//                                E_pArmObj->start_calibrate();
//                            } else {
//                                TRACES_WARNING_ARG1("Calibration stop  request for '%s'",E_pArmObj->name().c_str());
//                                E_pArmObj->stop_calibrate();
//                            }
//                            *retvalP = xmlrpc_c::value_int(0);
//
//                        } else {
//                            *retvalP = xmlrpc_c::value_int(-1);
//                        }
//                    }
//                    break;
//                default:
//                    *retvalP = xmlrpc_c::value_int(-2);
//                    throw "require only one parameter";
//                    break;
//            }
//            return;
//        }
//        
//};


/**
 * \class ArmSetCl
 * \brief 
 *
 */
class ArmSetCl : public xmlrpc_c::method
{
    public:
        ArmSetCl()
        {
            // this->_signature = "n:A";
            this->_help = "\n\
usage : xml.arm.set <longitude> <latitude> <radius>\n\n\
\tprovide positioning \n\
\n\n parameters:\n\
\t<longitude/latitude/radius> : \n\
\nreturn :\n\
\t<status> : 0 done / otherwise error\n\
\t\n\n";
            return;
        };
        
        void execute(xmlrpc_c::paramList const& paramList,xmlrpc_c::value *   const  retvalP) 
        {
            switch(paramList.size())
            {

                case 3:
                    {
                        // RtTrace * L_RtTraceBuffer;

                        tsArmPosition L_tsArmPosition;
                        L_tsArmPosition.longitude = paramList.getInt(0);
                        L_tsArmPosition.latitude  = paramList.getInt(1);
                        L_tsArmPosition.radius    = (unsigned int) paramList.getInt(2);
                        
                       
                        if (E_pArmObj != NULL)
                        {
                            TRACES_WARNING_ARG1("Set new pos request for '%s'",E_pArmObj->name().c_str());
                            E_pArmObj->set(L_tsArmPosition);
                            *retvalP = xmlrpc_c::value_int(0);

                        } else {
                            *retvalP = xmlrpc_c::value_int(-1);
                        }
                    }
                    break;
                default:
                    *retvalP = xmlrpc_c::value_int(-2);
                    throw "require only 3 parameters";
                    break;
            }
            return;
        }
        
};


void ArmRpcAttach(xmlrpc_c::registry * F_pRegistry)
{
    xmlrpc_c::methodPtr const ArmInfoObj  (new ArmInfoCl);
    // xmlrpc_c::methodPtr const ArmCalibrateObj(new ArmCalibrateCl);
    xmlrpc_c::methodPtr const ArmSetObj(new ArmSetCl);


    F_pRegistry->addMethod("xml.arm.info",      ArmInfoObj    );
    // F_pRegistry->addMethod("xml.arm.calibrate", ArmCalibrateObj  );
    F_pRegistry->addMethod("xml.arm.set",       ArmSetObj  );


    return ;
}

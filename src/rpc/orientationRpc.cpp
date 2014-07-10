/*!
 * \file orientationRpc.cpp
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
#include "orientation.h"
#include "rttrace.h"
#include "trace.h"

using namespace std;


static pthread_t G_OrientationTh;
static pthread_t G_OrientationCalibrationTh;

 /**
 * \class OrientationInfoCl
 * \brief 
 *
 */
class OrientationInfoCl : public xmlrpc_c::method
{
    public:
        OrientationInfoCl()
        {
            this->_signature = "n:A";
            this->_help = "\n\
usage : xml.orientation.info \n\n\
\treturn the list of existing Orientation or information if orientation <name> is filled \n\
\n\n parameters:\n\
\t<name>      : name of the orientation to get the information\n\
\nreturn :\n\
\t<list of Orientation name> \n\
\t<struct>  : struct of data for <name> Orientation \n\
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
                        tsOrientation L_tsOrientation;

                        if (E_pOrientationThObj != NULL)
                        {
                            // pair<string, xmlrpc_c::value> member_name("name", xmlrpc_c::value_string(E_pOrientationThObj->name()));
                            pair<string, xmlrpc_c::value> member_state("state", xmlrpc_c::value_string(ORIENTATION_KEYS[E_pOrientationThObj->state()]));
                            E_pOrientationThObj->get(&L_tsOrientation);
                            pair<string, xmlrpc_c::value> member_longitude("longitude", xmlrpc_c::value_int(L_tsOrientation.arm.longitude));
                            pair<string, xmlrpc_c::value> member_latitude("latitude", xmlrpc_c::value_int(L_tsOrientation.arm.latitude));
                            pair<string, xmlrpc_c::value> member_radius("radius", xmlrpc_c::value_int(L_tsOrientation.arm.radius));
                            // L_structData.insert(member_name);
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


/**
 * \class OrientationCalibrateCl
 * \brief 
 *
 */
class OrientationCalibrateCl : public xmlrpc_c::method
{
    public:
        OrientationCalibrateCl()
        {
            // this->_signature = "n:A";
            this->_help = "\n\
usage : xml.orientation.calibrate\n\n\
\tstart calibration for Orientation \n\
\n\n parameters:\n\
\t<start/stop> : \n\
\nreturn :\n\
\t<status> : 0 done / otherwise error\n\
\t\n\n";
            return;
        };
        
        void execute(xmlrpc_c::paramList const& paramList,xmlrpc_c::value *   const  retvalP) 
        {
            switch(paramList.size())
            {

                case 0:
                    {
                        
                        if (E_pOrientationThObj != NULL)
                        {
                            E_pOrientationThObj->stop();
                            pthread_create(&G_OrientationCalibrationTh, NULL, &OrientationThCl::calibrate, E_pOrientationThObj);        
                            *retvalP = xmlrpc_c::value_int(0);

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


/**
 * \class OrientationSetCl
 * \brief 
 *
 */
class OrientationSetCl : public xmlrpc_c::method
{
    public:
        OrientationSetCl()
        {
            // this->_signature = "n:A";
            this->_help = "\n\
usage : xml.orientation.set <longitude> <latitude> <radius>\n\n\
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
                       tsOrientation L_tsOrientation;
                       L_tsOrientation.arm.longitude = paramList.getInt(0);
                       L_tsOrientation.arm.latitude  = paramList.getInt(1);
                       L_tsOrientation.arm.radius    = (unsigned int) paramList.getInt(2);


                       if (E_pOrientationThObj != NULL)
                       {
                           // TRACES_WARNING_ARG1("Set new pos request for '%s'",E_pOrientationObj->name().c_str());
                           E_pOrientationThObj->set(L_tsOrientation);
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


/**
 * \class OrientationStartCl
 * \brief 
 *
 */
class OrientationStartCl : public xmlrpc_c::method
{
    public:
        OrientationStartCl()
        {
            // this->_signature = "n:A";
            this->_help = "\n\
usage : xml.orientation.stop \n\n\
\tstop thread \n\
\n\n parameters:\n\
\nreturn :\n\
\t<status> : 0 done / otherwise error\n\
\t\n\n";
            return;
        };
        
        void execute(xmlrpc_c::paramList const& paramList,xmlrpc_c::value *   const  retvalP) 
        {
            switch(paramList.size())
            {

               case 0:
                   {
                       if (E_pOrientationThObj != NULL)
                       {
                            if (E_pOrientationThObj->state() > ORIENTATION_CALIBRATE) 
                            {
                                TRACES_INFO("Create pthread for orientation");
                                pthread_create(&G_OrientationTh, NULL, &OrientationThCl::run, E_pOrientationThObj);        
                               *retvalP = xmlrpc_c::value_int(0);

                            }
                            else 
                            {
                                *retvalP = xmlrpc_c::value_int(-2);
                            }
                       } else {
                           *retvalP = xmlrpc_c::value_int(-1);
                       }
                   }
                   break;
                default:
                    *retvalP = xmlrpc_c::value_int(-2);
                    throw "require no parameters";
                    break;
            }
            return;
        }

};


/**
 * \class OrientationStopCl
 * \brief 
 *
 */
class OrientationStopCl : public xmlrpc_c::method
{
    public:
        OrientationStopCl()
        {
            // this->_signature = "n:A";
            this->_help = "\n\
usage : xml.orientation.stop \n\n\
\tstop thread \n\
\n\n parameters:\n\
\nreturn :\n\
\t<status> : 0 done / otherwise error\n\
\t\n\n";
            return;
        };
        
        void execute(xmlrpc_c::paramList const& paramList,xmlrpc_c::value *   const  retvalP) 
        {
            void * L_pvResult;
            switch(paramList.size())
            {
                case 0:
                    {
                        if (E_pOrientationThObj != NULL)
                        {
                            if (E_pOrientationThObj->state() != ORIENTATION_INIT) 
                            {
                                E_pOrientationThObj->stop();
                                pthread_join(G_OrientationTh,&L_pvResult);
                                *retvalP = xmlrpc_c::value_int(0);
                            }
                            else 
                            {
                                *retvalP = xmlrpc_c::value_int(-2);
                            }
                        } else {
                            *retvalP = xmlrpc_c::value_int(-1);
                        }
                    }
                    break;
                default:
                    *retvalP = xmlrpc_c::value_int(-2);
                    throw "require no parameters";
                    break;
            }
            return;
        }

};



void OrientationRpcAttach(xmlrpc_c::registry * F_pRegistry)
{
    xmlrpc_c::methodPtr const OrientationInfoObj  (new OrientationInfoCl);
    xmlrpc_c::methodPtr const OrientationCalibrateObj(new OrientationCalibrateCl);
    xmlrpc_c::methodPtr const OrientationSetObj(new OrientationSetCl);
    xmlrpc_c::methodPtr const OrientationStartObj(new OrientationStartCl);
    xmlrpc_c::methodPtr const OrientationStopObj(new OrientationStopCl);

    F_pRegistry->addMethod("xml.orientation.info",      OrientationInfoObj    );
    F_pRegistry->addMethod("xml.orientation.calibrate", OrientationCalibrateObj  );
    F_pRegistry->addMethod("xml.orientation.set",       OrientationSetObj  );
    F_pRegistry->addMethod("xml.orientation.start",     OrientationStartObj  );
    F_pRegistry->addMethod("xml.orientation.stop",      OrientationStopObj  );


    return ;
}

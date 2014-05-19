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
#include "ms_element.h"
#include "rttrace.h"
#include "trace.h"

using namespace std;


 /**
 * \class ElementInfoCl
 * \brief 
 *
 */
class ElementInfoCl : public xmlrpc_c::method
{
    public:
        ElementInfoCl()
        {
            this->_signature = "n:A";
            this->_help = "\n\
usage : xml.element.info [<name>] \n\n\
\treturn the list of existing element or information if element <name> is filled \n\
\n\n parameters:\n\
\t<name>      : name of the element to get the information\n\
\nreturn :\n\
\t<list of element name> \n\
\t<struct>  : struct of data for <name> element \n\
\t\n\n";
            return;
        };
        
        void execute(xmlrpc_c::paramList const& paramList,xmlrpc_c::value *   const  retvalP) 
        {

            switch(paramList.size())
            {
                case 0:
                    {
                        std::map<std::string, MotorSensorElementCl *>::iterator L_itElement ;
                        vector<xmlrpc_c::value> arrayData;
                        for (L_itElement = G_MapElementObj.begin() ; L_itElement != G_MapElementObj.end(); ++L_itElement)
                        {
                            // cout << *(L_itElement->second) << endl;
                            arrayData.push_back(xmlrpc_c::value_string(L_itElement->first));
                        }


                        xmlrpc_c::value_array array1(arrayData);
                        *retvalP = array1;
                    }
                    break;
                case 1:
                    {
                        // RtTrace * L_RtTraceBuffer;
                        std::string const L_name(paramList.getString(0));
                        std::map<std::string, MotorSensorElementCl *>::iterator L_itElement ;
                        map<string, xmlrpc_c::value> L_structData;
                        int L_direction;
                        
                        L_itElement = G_MapElementObj.find(L_name);
                        if (L_itElement != G_MapElementObj.end())
                        {
                            unsigned char L_u8SensorValue;
                            L_itElement->second->sensor(&L_u8SensorValue);
                            pair<string, xmlrpc_c::value> member_name("name", xmlrpc_c::value_string(L_itElement->first));
                            pair<string, xmlrpc_c::value> member_state("state", xmlrpc_c::value_int(L_itElement->second->state()));
                            pair<string, xmlrpc_c::value> member_speed("speed", xmlrpc_c::value_int(L_itElement->second->speed()));
                            pair<string, xmlrpc_c::value> member_current("current", xmlrpc_c::value_int(L_itElement->second->current()));
                            pair<string, xmlrpc_c::value> member_sensor("sensor", xmlrpc_c::value_int(L_u8SensorValue));
                            pair<string, xmlrpc_c::value> member_min("min", xmlrpc_c::value_int(L_itElement->second->min()));
                            pair<string, xmlrpc_c::value> member_max("max", xmlrpc_c::value_int(L_itElement->second->max()));
                            L_direction = (L_itElement->second->dir() == 1?CLOCKWISE:ANTICLOCKWISE);

                            pair<string, xmlrpc_c::value> member_dir("dir", xmlrpc_c::value_int(L_direction));
                            L_structData.insert(member_name);
                            L_structData.insert(member_state);
                            L_structData.insert(member_speed);
                            L_structData.insert(member_current);
                            L_structData.insert(member_sensor);
                            L_structData.insert(member_min);
                            L_structData.insert(member_max);
                            L_structData.insert(member_dir);
        
                            *retvalP =  xmlrpc_c::value_struct(L_structData);

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
 * \class ElementStepCl
 * \brief 
 *
 */
class ElementStepCl : public xmlrpc_c::method
{
    public:
        ElementStepCl()
        {
            // this->_signature = "n:A";
            this->_help = "\n\
usage : xml.element.step <name> <direction> [number]\n\n\
\tapply 1 to [number] step to direction \n\
\n\n parameters:\n\
\t<name>      : name of the element to apply the step(s)\n\
\t<direction> : 0 : CLOCKWISE / 1 : ANTICLOCKWISE\n\
\t<number>    : number of step to apply (default 1)\n\
\nreturn :\n\
\t<status> : 0 done / otherwise error\n\
\t\n\n";
            return;
        };
        
        void execute(xmlrpc_c::paramList const& paramList,xmlrpc_c::value *   const  retvalP) 
        {
            int L_i32Number(1);
            switch(paramList.size())
            {

                case 3:
                    cout << "case 3 /";
                    L_i32Number = paramList.getInt(2);
                case 2:
                    {
                        cout << "case 2 /";
                        // RtTrace * L_RtTraceBuffer;
                        std::string const L_name(paramList.getString(0));
                        int const L_dir(paramList.getInt(1));
                        std::map<std::string, MotorSensorElementCl *>::iterator L_itElement ;
                        map<string, xmlrpc_c::value> L_structData;
                        cout << L_name << " " << L_dir << " " << L_i32Number ;
                        
                        L_itElement = G_MapElementObj.find(L_name);
                        if (L_itElement != G_MapElementObj.end())
                        {   
                            int L_index;
                            L_itElement->second->dir((L_dir==1?-1:1));
                            for (L_index=0; L_index<L_i32Number; L_index++)
                            {
                                L_itElement->second->do_step();
                                cout << "." ;
                            }
                            cout <<  endl;
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
 * \class ElementCalibrateCl
 * \brief 
 *
 */
class ElementCalibrateCl : public xmlrpc_c::method
{
    public:
        ElementCalibrateCl()
        {
            // this->_signature = "n:A";
            this->_help = "\n\
usage : xml.element.calibrate <name> <start/stop>\n\n\
\tstart or stop calibration for element <name>\n\
\n\n parameters:\n\
\t<name>      : name of the element to apply the step(s)\n\
\t<start/stop> : number of step to apply (default 1)\n\
\nreturn :\n\
\t<status> : 0 done / otherwise error\n\
\t\n\n";
            return;
        };
        
        void execute(xmlrpc_c::paramList const& paramList,xmlrpc_c::value *   const  retvalP) 
        {
            switch(paramList.size())
            {

                case 2:
                    {
                        // RtTrace * L_RtTraceBuffer;
                        std::string const L_name(paramList.getString(0));
                        std::string const L_type(paramList.getString(1));
                        int L_start=0;
                        L_start= (L_type.compare("start") == 0);
                        
                        std::map<std::string, MotorSensorElementCl *>::iterator L_itElement ;
                        map<string, xmlrpc_c::value> L_structData;
                        
                        L_itElement = G_MapElementObj.find(L_name);
                        if (L_itElement != G_MapElementObj.end())
                        {
                            if (L_start==TRUE)
                            {
                                TRACES_WARNING_ARG1("Calibration start request for '%s'",L_itElement->first.c_str());
                                L_itElement->second->start_calibrate();
                            } else {
                                TRACES_WARNING_ARG1("Calibration stop  request for '%s'",L_itElement->first.c_str());
                                L_itElement->second->stop_calibrate();
                            }
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
 * \class ElementPosResetCl
 * \brief 
 *
 */
class ElementPosResetCl : public xmlrpc_c::method
{
    public:
        ElementPosResetCl()
        {
            // this->_signature = "n:A";
            this->_help = "\n\
usage : xml.element.pos.reset <name>\n\n\
\treset current position counter of element <name>\n\
\n\n parameters:\n\
\t<name>      : name of the element to apply the step(s)\n\
\nreturn :\n\
\t<status> : 0 done / otherwise error\n\
\t\n\n";
            return;
        };
        
        void execute(xmlrpc_c::paramList const& paramList,xmlrpc_c::value *   const  retvalP) 
        {
            switch(paramList.size())
            {

                case 1:
                    {
                        // RtTrace * L_RtTraceBuffer;
                        std::string const L_name(paramList.getString(0));
                        std::map<std::string, MotorSensorElementCl *>::iterator L_itElement ;
                        
                        L_itElement = G_MapElementObj.find(L_name);
                        if (L_itElement != G_MapElementObj.end())
                        {
                            L_itElement->second->current(0);
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
 * \class ElementPosMinCl
 * \brief 
 *
 */
class ElementPosMinCl : public xmlrpc_c::method
{
    public:
        ElementPosMinCl()
        {
            // this->_signature = "n:A";
            this->_help = "\n\
usage : xml.element.pos.min <name> <value>\n\n\
\tset min value for stepper range for element <name>\n\
\n\n parameters:\n\
\t<name>      : name of the element to apply the step(s)\n\
\t<value>     : min value\n\
\nreturn :\n\
\t<status> : 0 done / otherwise error\n\
\t\n\n";
            return;
        };
        
        void execute(xmlrpc_c::paramList const& paramList,xmlrpc_c::value *   const  retvalP) 
        {
            switch(paramList.size())
            {

                case 2:
                    {
                        // RtTrace * L_RtTraceBuffer;
                        std::string const L_name(paramList.getString(0));
                        int const L_value(paramList.getInt(1));
                        
                        std::map<std::string, MotorSensorElementCl *>::iterator L_itElement ;
                        map<string, xmlrpc_c::value> L_structData;
                        
                        L_itElement = G_MapElementObj.find(L_name);
                        if (L_itElement != G_MapElementObj.end())
                        {
                            L_itElement->second->min(L_value);
                            *retvalP = xmlrpc_c::value_int(0);

                        } else {
                            *retvalP = xmlrpc_c::value_int(-1);
                        }
                    }
                    break;
                default:
                    *retvalP = xmlrpc_c::value_int(-2);
                    throw "require only 2 parameters <name> <value>";
                    break;
            }
            return;
        }
        
};

/**
 * \class ElementPosMaxCl
 * \brief 
 *
 */
class ElementPosMaxCl : public xmlrpc_c::method
{
    public:
        ElementPosMaxCl()
        {
            // this->_signature = "n:A";
            this->_help = "\n\
usage : xml.element.pos.min <name> <value>\n\n\
\tset min value for stepper range for element <name>\n\
\n\n parameters:\n\
\t<name>      : name of the element to apply the step(s)\n\
\t<value>     : min value\n\
\nreturn :\n\
\t<status> : 0 done / otherwise error\n\
\t\n\n";
            return;
        };
        
        void execute(xmlrpc_c::paramList const& paramList,xmlrpc_c::value *   const  retvalP) 
        {
            switch(paramList.size())
            {

                case 2:
                    {
                        // RtTrace * L_RtTraceBuffer;
                        std::string const L_name(paramList.getString(0));
                        int const L_value(paramList.getInt(1));
                        
                        std::map<std::string, MotorSensorElementCl *>::iterator L_itElement ;
                        map<string, xmlrpc_c::value> L_structData;
                        
                        L_itElement = G_MapElementObj.find(L_name);
                        if (L_itElement != G_MapElementObj.end())
                        {
                            L_itElement->second->max(L_value);
                            *retvalP = xmlrpc_c::value_int(0);

                        } else {
                            *retvalP = xmlrpc_c::value_int(-1);
                        }
                    }
                    break;
                default:
                    *retvalP = xmlrpc_c::value_int(-2);
                    throw "require only 2 parameters <name> <value>";
                    break;
            }
            return;
        }
        
};

void ElementRpcAttach(xmlrpc_c::registry * F_pRegistry)
{
    xmlrpc_c::methodPtr const ElementInfoObj  (new ElementInfoCl);
    xmlrpc_c::methodPtr const ElementStepObj  (new ElementStepCl);
    xmlrpc_c::methodPtr const ElementCalibrateObj(new ElementCalibrateCl);
    xmlrpc_c::methodPtr const ElementPosResetObj(new ElementPosResetCl);
    xmlrpc_c::methodPtr const ElementPosMinObj(new ElementPosMinCl);
    xmlrpc_c::methodPtr const ElementPosMaxObj(new ElementPosMaxCl);

    F_pRegistry->addMethod("xml.element.info",   ElementInfoObj    );
    F_pRegistry->addMethod("xml.element.step",   ElementStepObj    );
    F_pRegistry->addMethod("xml.element.calibrate", ElementCalibrateObj  );
    F_pRegistry->addMethod("xml.element.pos.reset", ElementPosResetObj  );
    F_pRegistry->addMethod("xml.element.pos.min", ElementPosMinObj  );
    F_pRegistry->addMethod("xml.element.pos.max", ElementPosMaxObj  );

    return ;
}

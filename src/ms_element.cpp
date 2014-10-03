/*!
 * \file _ms_element.cpp
 * \author claude.bideau@gmail.com
 * \version 0.1
 * \brief This module provides a motor/sensor declaration.
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
 * \section DESCRIPTION
 *
 * This module provides a motor/sensor declaration.
 *
 */
#include <iostream>
#include <list>
#include <map>
#include "trace.h"
#include "ms_element.h"

using namespace std;

std::map<std::string, MotorSensorElementCl *> G_MapElementObj;


/**
   @brief construct

   @param[in]     _pini    ini Object from inifile parsing
   @param[in]     _keyName motor/Sensor name
   
   @return none.
 */
MotorSensorElementCl::MotorSensorElementCl(iniCl * _pIni, std::string _keyName)
{
    std::list<std::string> F_listKey;
    std::list<std::string>::iterator L_it;
    std::string  L_driverName;
    std::string  L_sensorName;
    long  L_s64sensorIo;
    long  L_s64Value;
    std::vector<int> L_vectorValue;

    TRACES_MSG_ARG1("MotorSensorElementCl: %s",_keyName.c_str());

    _state  = MS_INIT;
    _name   = _keyName;

    /* create and associate Rt Trace object */
    _trace = new RtTrace(_keyName, RT_TRACE_ENABLE |  MOTOR_CALIBRATION | MOTOR_STEP);
    _trace->trace(0, RT_TRACE_ENABLE, 0xFFFFFFFF, 2);
    TRACES_MSG_ARG1("MotorSensorElementCl: %s",_keyName.c_str());

    _minStep          = -1<<31;
    _minAngle         = -1<<31;
    _maxStep          = -1<<31;
    _maxAngle         = -1<<31;
    _calibrationAngle = -1<<31;
    _currentStep      = 0;
    _requireStep      = 0;
    /* get parameters */
    
    TRACES_MSG_ARG1("MotorSensorElementCl: %s ini parsing",_keyName.c_str());
    F_listKey = _pIni->getItems(_name);

    if (!F_listKey.empty())
    {
        /* find driver/sensor */
        L_driverName = _pIni->get(_name,"driver");
        _pDriver = new EasyDriverCl(_pIni,L_driverName);
        TRACES_MSG_ARG1("MotorSensorElementCl: %s eo driver ini parsing",_keyName.c_str());
        L_sensorName = _pIni->get(_name,"sensor");
        TRACES_MSG_ARG1("MotorSensorElementCl: %s eo sensor ini parsing",_keyName.c_str());
        if (L_sensorName.size() >= 0)
        {
            L_s64sensorIo = _pIni->getInteger(L_sensorName,"input",-1);
            if (L_s64sensorIo > 0)
                _pSensor = new GpioInCl((unsigned int) L_s64sensorIo);
        }
        /* range */
        L_vectorValue = _pIni->getIntVector(_name,"min",- 1<<31);
        if (L_vectorValue.size() != 2) throw std::string("MotorSensorElementCl: ") + _name + std::string(": min option require only 2 integer / min = <step> <degre>");
        _minStep   = L_vectorValue[0];
        _minAngle  = L_vectorValue[1];

        L_vectorValue = _pIni->getIntVector(_name,"max",- 1<<31);
        if (L_vectorValue.size() != 2) throw std::string("MotorSensorElementCl: ") + _name + std::string(": max option require only 2 integer / max = <step> <degre>");
        _maxStep   = L_vectorValue[0];
        _maxAngle  = L_vectorValue[1];
        
        L_s64Value = _pIni->getInteger(_name,"calibration",- 1<<31);
        if (L_s64Value == -1<<31)  throw std::string("MotorSensorElementCl: ") + _name + std::string(": calibration angle not define / calibration= <degre>");
        _calibrationAngle = (int) L_s64Value;

        L_s64Value = _pIni->getInteger(_name,"speed", 100);
        if (L_s64Value > 0) _speed = L_s64Value;
        _pDriver->setSpeed(_speed);
        
        _anglePreProc();

        
    } else throw  std::string("undefined element : ")+_name;
    
    TRACES_MSG_ARG1("MotorSensorElementCl: %s eo ini parsing",_keyName.c_str());
    _pDriver->disable();
    _pDriver->sleep();

    // /* find the dependance if exist */
    // _pAttachedMS = NULL;
    // std::string L_dependName(_pIni->get(_name,"depend"));
    // if (L_dependName.compare("") != 0)
    // {
        // std::map<std::string, MotorSensorElementCl *>::iterator L_itElement ;
        // L_itElement = G_MapElementObj.find(L_dependName);
        // if (L_itElement != G_MapElementObj.end())
        // {
            // /* manage the dependence */
            // L_itElement->second->attach(this);
        // }
        // else {
            // TRACES_ERROR_ARG1("there is a dependence with '%s'",L_dependName.c_str());
            // throw "there is a dependence with " +L_dependName;
        // }
    // }
    

    _teValuePrev = _pSensor->get();
    _bDetectEdge=false;
    _state  = MS_UNCALIBRATE;
}


/**
   @brief attached external motor sensor element
   @return None
 */

// void MotorSensorElementCl::attach(MotorSensorElementCl * _pAttachedElt)
// {
    // _pAttachedMS = _pAttachedElt;
    // return;
// }


/**
   @brief return the name of the element
   @return motor/sensor name.
 */

std::string MotorSensorElementCl::name(void)
{
    return _name;
}

/**
   @brief enable the stepper driver
   @return none.
 */
void MotorSensorElementCl::enable(void)
{
    _pDriver->enable();
}

/**
   @brief disable the stepper driver
   @return none.
 */
void MotorSensorElementCl::disable(void)
{
    _pDriver->disable();
}

/**
   @brief force to sleep the stepper driver
   @return none.
 */
void MotorSensorElementCl::sleep(void)
{
    _pDriver->sleep();
}

/**
   @brief wakeup the stepper driver
   @return none.
 */
void MotorSensorElementCl::wakeup(void)
{
    _pDriver->wakeup();
}

/**
   @brief wakeup the stepper driver
   @return none.
 */
teMSState MotorSensorElementCl::state(void)
{
    return _state;
}

/**
   @brief set the speed of the stepper driver
   @return none.
 */
unsigned int MotorSensorElementCl::speed(unsigned int F_u32speed)
{
    _pDriver->setSpeed(F_u32speed);
    return _pDriver->speed;
    
}

/**
   @brief return the speed of stepper driver
   @return speed value
 */

unsigned int MotorSensorElementCl::speed(void)
{
    return _pDriver->speed;
}


/**
   @brief return the speed mapping mask of stepper driver
   @return speed value
 */

unsigned int MotorSensorElementCl::speed_mask(void)
{
    return _pDriver->speed_mask;
}


/**
   @brief calibrate the position of the motor with sensor edge
   @return none.
 */

bool MotorSensorElementCl::calibrate(void)
{
    teValue _teValuePrev;
    int     L_i32Step;
    
    if (_bDetectEdge==false)
    {
        _teValueCurrent = _pSensor->get();
        if (_teValueCurrent != _teValuePrev)
        {
            _bDetectEdge=true;
            _trace->trace(MOTOR_CALIBRATION, MOTOR_CALIBRATION, _teValueCurrent, _teValuePrev );
            _state  = MS_READY;
            _calibrationStep = 0;
        } else 
        {
            L_i32Step = ( _teValueCurrent == LOW) ? 1 : -1;
            /* do step */
            _trace->trace(MOTOR_CALIBRATION, MOTOR_CALIBRATION, _teValueCurrent, _teValuePrev );
            _pDriver->step_pulse(L_i32Step);

        }
    }
#ifdef __TEST
    /* force to true in case of test mode */
    _bDetectEdge = true;
#endif
    return _bDetectEdge;
}

/**
   @brief       set the value to target
   @param[in]   value
   @return none.
 */
void MotorSensorElementCl::set(int _i32Value)
{
    // input value is scaled 
#if 1
    _requireStep += _i32Value <<7;
#else
    _requireStep = _i32Value;
#endif

#if 0
   /* CB code */
   //_requireStep = (_i32Value * _factorStep) >> PI_SCALED_POWER;
#endif
    //std::cout << "_requireStep="<<_requireStep<< endl;
}

/**
   @brief       set the value to target
   @return none.
 */
void MotorSensorElementCl::rise()
{
#if 1
    int L_i32diff;

    L_i32diff = _requireStep- _currentStep;
    _direction = (L_i32diff >=0) ? 1: -1;
    if (L_i32diff!=0)
    {
        _pDriver->step_pulse(_direction);
        _currentStep += _direction;
        // std::cout<<_name << "   required:"<<_requireStep<<"\tcurrent:"<<_currentStep<<endl;
   }

#else
   _direction = (_requireStep >=0) ? 1: -1;
    if (_requireStep!=0)
    {
        _pDriver->step_pulse(_direction);
        _currentStep += _direction;
        // std::cout<<_name << "   required:"<<_requireStep<<"\tcurrent:"<<_currentStep<<endl;
        _requireStep=0;
   }
#endif

}

/**
   @brief       set the value to target
   @param[in]   value
   @return none.
 */
void MotorSensorElementCl::fall()
{
    _pDriver->step_fall();
}


/**
   @brief       set the direction
   @param[in]   _dirValue direction value -1/1
   @return none.
 */
void MotorSensorElementCl::dir(int _dirValue)
{
    _direction = _dirValue;
    // if (_dirValue > 0)
        // _direction = -1;  /* ANTICLOCKWISE */
    // else
        // _direction = 1;   /* CLOCKWISE */
    return;
}

/**
   @brief       do a step pulse in current direction
   @return none.
 */
void MotorSensorElementCl::do_step(void)
{
    // int step = (_direction>0)?1:-1;
    _pDriver->step_pulse(_direction);
    _currentStep += _direction;
    usleep(100);
    fall();
    return;
}

/**
   @brief       set current position
   @return .
 */
void MotorSensorElementCl::current(int _value)
{
    _currentStep =_value;
}


/**
   @brief       return current position
   @return .
 */
int MotorSensorElementCl::current(void)
{
    return _currentStep;
}

/**
   @brief return the speed of stepper driver
   @return speed value
 */

  /**
   @brief       set min position
   @return .
 */
void MotorSensorElementCl::min(int _value)
{
    _minStep =_value;
}

/**
   @brief       set min position and angle
   @return .
 */
void MotorSensorElementCl::min(int _value,int _angle)
{
    _minStep  = _value;
    _minAngle = _angle;
    _anglePreProc();
}


/**
   @brief       return min position
   @return .
 */
int MotorSensorElementCl::min(void)
{
    return _minStep;
}


/**
   @brief       get min position and angle
   @return .
 */
void MotorSensorElementCl::min(int * _pvalue,int * _pangle)
{
    (*_pvalue) = _minStep;
    (*_pangle) = _minAngle;
}

/**
   @brief       set max position
   @return .
 */
void MotorSensorElementCl::max(int _value)
{
    _maxStep =_value;
}

/**
   @brief       set max position and angle
   @return .
 */
void MotorSensorElementCl::max(int _value,int _angle)
{
    _maxStep  = _value;
    _maxAngle = _angle;
    _anglePreProc();
}


/**
   @brief       return max position
   @return .
 */
int MotorSensorElementCl::max(void)
{
    return _maxStep;
}

/**
   @brief       get max position and angle
   @return .
 */
void MotorSensorElementCl::max(int * _pvalue,int * _pangle)
{
    (*_pvalue) = _maxStep;
    (*_pangle) = _maxAngle;
}

/**
   @brief       get max position and angle
   @return .
 */
void MotorSensorElementCl::factor(int * F_pi32step)
{
    (*F_pi32step)  = _factorStep;
}

/**
   @brief return the speed of stepper driver
   @return speed value
 */

void MotorSensorElementCl::sensor(unsigned char * _pSensorValue)
{
    // cout << "read sensor(" << _pSensor->id << ")" << endl;
    (*_pSensorValue)= _pSensor->get();
}

/**
   @brief return the speed of stepper driver
   @return speed value
 */

int MotorSensorElementCl::dir(void)
{
    _direction = (_pDriver->dir()==CLOCKWISE?1:-1);
    return  _direction;
}


/**
   @brief       set ini parameters
   @param[in]   _param parameter to update ( min/max/ )
   @param[in]   _value new value
   @return .
 */
void MotorSensorElementCl::ini(std::string _param, int _value)
{
    
}

void MotorSensorElementCl::_anglePreProc(void)
{
    /* angle pre-processing to facilitate angle determination */
    int    L_i32diffStep, L_i32diffAngle;
    L_i32diffStep  = _maxStep  - _minStep;
    L_i32diffAngle = _maxAngle - _minAngle;
    
    if (L_i32diffAngle == 0) return;

    /* determine pi/2 step factor */
    /*  nbstep = (_factorStep* scaledAngle) >>15   */
    _factorStep     = (90*L_i32diffStep)/L_i32diffAngle;
    // cout << "L_i32diffStep  =" << L_i32diffStep  << endl;
    // cout << "L_i32diffAngle =" << L_i32diffAngle << endl;
    // cout << "_factorStep    =" << _factorStep    << endl;

    return;
}


MotorSensorElementCl::~MotorSensorElementCl(void)
{

    delete _pDriver;
    delete _pSensor;
}


/*!
 * \file IssueException.h
 * \author claude.bideau@gmail.com
 * \version 0.1
 * \brief This module provides Excepyion Mechanism declaration.
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
 * This module provides INI parser declaration.
 *
 */
#ifndef __ISSUE_EXCEPTION_H
#define __ISSUE_EXCEPTION_H

#include <exception>

using namespace std;
 
class IssueCl: public exception
{
    public:
        IssueCl(int service=0, string const& sentence="") throw()
             :_service(service),_sentence(sentence)
        {}
     
         virtual const char* what() const throw()
         {
             return _sentence.c_str();
         }
         
         int getService() const throw()
         {
              return _service;
         }
        
        virtual ~IssueCl() throw()
        {}
     
    private:
        int    _service;             //Identity of the service 
        string _sentence;            //Description of the issue
};

#endif


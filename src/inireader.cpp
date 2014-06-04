/*!
 * \file inireader.cpp
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
#include <sstream> 
#include <fstream>
#include "inireader.h"

using namespace std;



// std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems)
// {
    // std::stringstream ss(s);
    // std::string item;
    // while (std::getline(ss, item, delim)) {
        // elems.push_back(item);
    // }
    // return elems;
// }


// std::vector<std::string> split(const std::string &s, char delim)
// {
    // std::vector<std::string> elems;
    // split(s, delim, elems);
    // return elems;
// }

vector<string> split(const string& s, const string& delim, const bool keep_empty)
{
    vector<string> result;
    if (delim.empty()) {
        result.push_back(s);
        return result;
    }
    string::const_iterator substart = s.begin(), subend;
    while (true) {
        subend = search(substart, s.end(), delim.begin(), delim.end());
        string temp(substart, subend);
        if (keep_empty || !temp.empty()) {
            result.push_back(temp);
        }
        if (subend == s.end()) {
            break;
        }
        substart = subend + delim.size();
    }
    return result;
}


iniSectionCl::iniSectionCl(void):
   _name("") 
{
     // std::cerr << "iniSectionCl() called"
               // << " -- this=" << this << ". [default ctor]" << std::endl;
}

iniSectionCl::iniSectionCl(std::string name)
{
    _name = name;
}

iniSectionCl::iniSectionCl(const iniSectionCl &copy)
{
    _name = copy._name;
    // std::cerr << "iniSectionCl(" << &copy << ") called"
              // << " -- this=" << this << ". [copy ctor]" << std::endl;
}

iniSectionCl& iniSectionCl::operator=(const iniSectionCl &rhs) 
{
    _name = rhs._name;
    // std::cerr << "operator=(" << &rhs << ") called"
              // << " -- this=" << this << ". [assign operator]" << std::endl;
    return *this;
}

std::string iniSectionCl::getName() { return _name; }

void iniSectionCl::add_item(std::string name, std::string value)
{
     _items[name] = value;   
}


std::list<std::string> iniSectionCl::keys()
{
    std::list<std::string> L_list;
    std::map<std::string, std::string>::const_iterator it;
    for (it = _items.begin(); it != _items.end(); ++it)
    {
        L_list.push_back(it->first);
    }
    return L_list;
}

std::string iniSectionCl::get(std::string name)
{
	if (_items.find(name)  != _items.end())
		return _items[name];

	return "";
}

iniSectionCl::~iniSectionCl(void)
{
    std::cout << "====== DELETE iniSectionCl "<< _name << "======"<<endl;
    _items.erase(_items.begin(),_items.end() );
}


iniCl::iniCl(std::string F_filename)
{
    inifile = F_filename;
    _parse();
}


void iniCl::_parse(void)
{
    ifstream L_fd;
    std::string L_line;
    std::size_t firstSect;
    std::size_t endSect;
    std::size_t firstDelim;
    std::string L_section;
    std::string L_key;
    std::string L_value;
    
    L_fd.open(inifile.c_str());
    if ( ! L_fd.is_open() )  throw std::string("Error: impossible to open ")+inifile;
    
    while( std::getline( L_fd, L_line ) )
    {
        L_line = ltrim(L_line);
        L_line = rtrim(L_line);
        if (!L_line.empty())
        {
            // cout << L_line<< endl;
            // clean comment
            // 2 simple cases : start with ';' or '#'
            if ((L_line[0] == ';')||(L_line[0] == '#'))
            {   // comment line => suppress it
            } else if (L_line[0] == '[')
            {   // section =>
 
                // cout << "Section" <<endl;
                firstSect = L_line.find("[");
                endSect = L_line.find("]");
                if (endSect!=std::string::npos)
                {
                    // extract section name 
                    L_section = L_line.substr(firstSect+1,endSect-1);
                    add_section(L_section);
                }
            } else {
                firstDelim = L_line.find(":");
                if (firstDelim!=std::string::npos)
                {
                    L_value = L_line.substr(firstDelim+1,std::string::npos);
                    L_key = L_line.substr(0,firstDelim);
                    // cout << "\"" << L_key << "\" = \"" << L_value << "\"" << endl;
                    add_item(L_section,L_key,L_value);
                } else {
                    firstDelim = L_line.find("=");
                    if (firstDelim!=std::string::npos)
                    {
                        L_value = L_line.substr(firstDelim+1,std::string::npos);
                        L_key = L_line.substr(0,firstDelim);
                        // cout << "\"" << L_key << "\" = \"" << L_value << "\"" << endl;
                        add_item(L_section,L_key,L_value);
                    }
                }
            }
        }
    }    
    return;
}

void iniCl::add_section(std::string F_name)
{
    iniSectionCl * section = new iniSectionCl(F_name);
    lastSection = F_name;
    _sections[lastSection] = section;
}


void iniCl::add_item(std::string name, std::string key, std::string value )
{
    _sections[lastSection]->add_item(key,value);
}

std::list<std::string> iniCl::sections(void)
{
    std::list<std::string> L_list;
    std::map<std::string, iniSectionCl *>::const_iterator it;
    for (it = _sections.begin(); it != _sections.end(); ++it)
    {
        L_list.push_back(it->first);
    }
    return L_list;
}


void iniCl::getSections(void)
{
    std::map<std::string, iniSectionCl *>::const_iterator it;
    std::cout << "============================================="<<endl;
    for (it = _sections.begin(); it != _sections.end(); ++it)
    {
        std::cout << "iniCl(" << it->first << "), " << it->second->getName() << std::endl;
    }
    std::cout << "============================================="<<endl;

}


std::list<std::string> iniCl::getItems(std::string F_section)
{
	std::list<std::string> L_list;
	if (_sections.find(F_section)  != _sections.end())
		L_list= _sections[F_section]->keys();
	return L_list;
    // return NULL;

}

std::string iniCl::get(std::string F_section,std::string F_key)
{

	if (_sections.find(F_section)  != _sections.end())
	{
		return _sections[F_section]->get(F_key);
	}
	return "";

}

std::vector<std::string> iniCl::getStrVector(std::string F_section,std::string F_key)
{
    std::vector<std::string> L_vector;
    std::string L_value;

	if (_sections.find(F_section)  != _sections.end())
	{
        L_value = _sections[F_section]->get(F_key);
        return split( L_value," ", false);
	}
	return L_vector;

}

std::vector<int> iniCl::getIntVector(std::string F_section,std::string F_key, long default_value)
{
    std::vector<std::string> L_strVector;
	std::vector<std::string>::iterator L_it ;
    
    std::vector<int> L_i32Vector;
    std::string L_value;

    L_strVector = getStrVector(F_section,F_key);
    for (L_it = L_strVector.begin() ; L_it != L_strVector.end(); ++L_it)
    {
        // std::cout << " '" << *L_it << "'";
        std::string L_strValue = *L_it;
        const char* value = L_strValue.c_str();
        char* end;
        long n = strtol(value, &end, 0);
        L_i32Vector.push_back( end > value ? n : default_value);

    }
	if (_sections.find(F_section)  != _sections.end())
    
	return L_i32Vector;

}


long iniCl::getInteger(std::string F_section,std::string F_key, long default_value)
{
    string L_valstr = get(F_section,F_key);

	// std::cout << "integer= '" <<L_valstr << "' size= " <<L_valstr.size() << endl;
	if (L_valstr.size() == 0) return default_value;

    const char* value = L_valstr.c_str();
    char* end;
    // This parses "1234" (decimal) and also "0x4D2" (hex)
    long n = strtol(value, &end, 0);
    // std::cout << "n=" <<n << endl;
    return end > value ? n : default_value;
}

double iniCl::getReal(std::string F_section,std::string F_key, double default_value)
{
    string L_valstr = _sections[F_section]->get(F_key);
    const char* value = L_valstr.c_str();
    char* end;
    double n = strtod(value, &end);
    return end > value ? n : default_value;
}

bool iniCl::getBoolean(std::string F_section,std::string F_key, bool default_value)
{
    string L_valstr = _sections[F_section]->get(F_key);
    // Convert to lower case to make string comparisons case-insensitive
    std::transform(L_valstr.begin(), L_valstr.end(), L_valstr.begin(), ::tolower);
    if (L_valstr == "true" || L_valstr == "yes" || L_valstr == "on" || L_valstr == "1")
        return true;
    else if (L_valstr == "false" || L_valstr == "no" || L_valstr == "off" || L_valstr == "0")
        return false;
    else
        return default_value;
}


iniCl::~iniCl()
{
    // std::cout << "====== DELETE iniCl ======"<<endl;
    _sections.erase(_sections.begin(),_sections.end() );
    getSections();
}

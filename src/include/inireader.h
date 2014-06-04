/*!
 * \file ini.h
 * \author claude.bideau@gmail.com
 * \version 0.1
 * \brief This module provides INI parser declaration.
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
 #ifndef __INIREADER_H
#define __INIREADER_H

/**   1. Standard Library Include                                   **/
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <vector>
#include <algorithm> 

using namespace std;

/**   1. Include files  (own)                                       **/
/**   2a.   External Functions                                      **/
/**   2b.   External Data                                           **/
/**   3.  Type Definitions                                          **/
/**   5.  Local Functions                                           **/
/**   6.  Macros / Defines                                          **/

// trim from start
static inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
        return ltrim(rtrim(s));
}

// extern std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
// extern std::vector<std::string> split(const std::string &s, char delim);
extern vector<string> split(const string& s, const string& delim, const bool keep_empty);

 /**
 * \class
 * \brief 
 *
 */

 
class iniSectionCl
{
    public:
        iniSectionCl();
        iniSectionCl(std::string);
        iniSectionCl(const iniSectionCl &);
        iniSectionCl& operator=(const iniSectionCl &rhs);

        std::list<std::string> keys(void);

        void add_item(std::string, std::string);
        // std::map<std::string, std::string> items();
        std::string getName();
        std::string get(std::string);
        virtual ~iniSectionCl();
    protected:
    private:
        std::map<std::string, std::string> _items;
        std::string _name;

};


class iniCl
{
    public:
        iniCl(std::string);
        // std::map<std::string, iniSectionCl> sections();
        void getSections();
        std::list<std::string> sections(void);
        std::list<std::string> getItems(std::string);
        std::string get(std::string, std::string );
        std::vector<std::string> getStrVector(std::string, std::string );
        std::vector<int> getIntVector(std::string ,std::string , long );
        long getInteger(std::string, std::string , long );
        double getReal(std::string, std::string, double );
        bool getBoolean(std::string, std::string, bool );


        virtual ~iniCl();
    protected:
    private:
        void _parse(void);
        void add_section(std::string );
        void add_item(std::string , std::string , std::string  );

        std::string inifile;
        std::map<std::string, iniSectionCl *>  _sections;
        std::string lastSection;

};


static inline void iniClDisplay( iniCl * F_iniCl)
{
    std::list<std::string> section_list;
    std::list<std::string> item_list;
    std::list<std::string>::const_iterator it;
    std::list<std::string>::const_iterator it2;
    
    section_list = F_iniCl->sections();
    cout << "====Sections list ===================="<<endl;
    for (it = section_list.begin(); it != section_list.end(); ++it)
    {
        cout << "section = " << *it << endl;
    }
    cout << "====item list ===================="<<endl;
    for (it = section_list.begin(); it != section_list.end(); ++it)
    {
        item_list = F_iniCl->getItems(*it);
        for (it2 = item_list.begin(); it2 != item_list.end(); ++it2)
        {
            cout << "section["<< *it <<"][" <<*it2 << "] = '" << F_iniCl->get(*it,*it2) << "'" << endl;
        }
    }
}

#endif // __INI_H

// $Id: create_parse.h,v 1.5 2004/11/12 07:41:44 christof Exp $
/*  ManuProC_Base: Main ManuProC Library
 *  Copyright (C) 2004  Christof Petig
 *  Copyright (C) 2010 Christof Petig
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef D086626E_F86A_453E_8C15_0B36F04A5CAD
#define D086626E_F86A_453E_8C15_0B36F04A5CAD

#include <string>
#include <stdexcept>

namespace ManuProC
{       template <class T>
    	 T parse(const std::string &val);
    	template <class T>
    	 static T parse_def(const std::string &val, const T &def=T());
    	template <class T>
    	 std::string create(const T &val);
}

namespace EntryValue_easy { struct nil; }

// you can skip these declarations while reading this file
namespace ManuProC {
template <> std::string create<int>(const int &val);
template <> std::string create<double>(const double &val);
template <> std::string create<bool>(const bool &val);
template <> int parse<int>(const std::string &value);
template <> bool parse<bool>(const std::string &value);
template <> long parse<long>(const std::string &value);
template <> double parse<double>(const std::string &value);
template <> float parse<float>(const std::string &value);
template <> std::string parse<std::string>(const std::string &value);
template <> EntryValue_easy::nil parse<EntryValue_easy::nil>(const std::string &value);
}

// g++ 2.95 does not like these inlined
namespace ManuProC {
template <class T>
 T parse_def(const std::string &val, const T &def)
{  try { return parse<T>(val); } 
   catch (std::out_of_range &e) { return def; }
}
}

#endif

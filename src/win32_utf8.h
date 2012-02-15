/*  libcommonc++: ManuProC's main OO library
 *  Copyright (C) 2012 Christof Petig
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

#include <string>

#ifdef WIN32
namespace ManuProC {
std::wstring make_wstring(std::string const& x);
std::string un_wstring(std::wstring const& x);
}

#define access access_utf8
extern "C" int access_utf8(const char*, int);

#define fopen fopen_utf8
# ifdef _FILE_DEFINED
extern "C" FILE* fopen_utf8(const char*, const char*);
# endif

#endif

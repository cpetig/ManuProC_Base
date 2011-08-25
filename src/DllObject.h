/*  libcommonc++: ManuProC's main OO library
 *  Copyright (C) 2011 Christof Petig
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

struct DllObjectBase;

// creation func signature: DllObjectBase *()(void const *);  // we might need to pass module handle to creation function
DllObjectBase *CreateDllObject(std::string const& library, std::string const& creation_func, void const*user_data=0);

void UnloadDllObject(DllObjectBase*); //!< destroy object and unload library

std::string SearchDll(std::string const& name);

struct DllObjectBase
{
protected:
  friend void UnloadDllObject(DllObjectBase*);
  friend DllObjectBase *CreateDllObject(std::string const& library, std::string const& creation_func, void const*user_data);
  void * systemhandle;
  virtual ~DllObjectBase() {}
  DllObjectBase() : systemhandle() {}
public:
  virtual void destroy(); // { delete this; }
};

#ifdef WIN32
namespace ManuProC {
std::wstring make_wstring(std::string const& x);
std::string un_wstring(std::wstring const& x);
}
#endif
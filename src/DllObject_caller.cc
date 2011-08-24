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

#include "DllObject.h"

#ifdef WIN32
# include <assert.h>
# include "windows.h"

std::wstring ManuProC::make_wstring(std::string const& x)
{
  if (x.empty()) return std::wstring();
  wchar_t wstring[10240];
  int res= MultiByteToWideChar(CP_UTF8, 0, x.data(), x.size(), wstring, sizeof(wstring)/sizeof(wchar_t));
  assert(res!=0);
//  size_t r= mbstowcs(wstring,x.c_str(),sizeof(wstring)/sizeof(wchar_t));
//  if (r==(size_t)(-1)) return std::wstring();
  return std::wstring(wstring,wstring+res);
}

std::string ManuProC::un_wstring(std::wstring const& x)
{
  if (x.empty()) return std::string();
  char nstring[10240];
  int res= WideCharToMultiByte(CP_UTF8, 0, x.data(), x.size(), nstring, sizeof(nstring), NULL, NULL);
  assert(res!=0);
//  size_t r= wcstombs(nstring,x.c_str(),sizeof(nstring)/sizeof(char));
//  if (r==(size_t)(-1)) return std::string();
  return std::string(nstring,nstring+res);
}
#else
#include <dlfcn.h>
#endif

typedef DllObjectBase *(*creationfunptr)(void const*);

DllObjectBase *CreateDllObject(std::string const& library, std::string const& creation_func, void const*user_data)
{
  DllObjectBase *res=0;
  creationfunptr fun=0;
#ifdef WIN32
  HMODULE lib=LoadLibraryW(ManuProC::make_wstring(library).c_str());
  if (!lib) return 0;
  fun= (creationfunptr)GetProcAddress(lib, creation_func.c_str());
  if (!fun)
  {
    FreeLibrary(lib);
    return 0;
  }
  res=(*fun)(user_data);
  if (!res)
  {
    FreeLibrary(lib);
    return 0;
  }
  res->systemhandle=lib;
#else
  void *handle=dlopen(library.c_str(), RTLD_NOW|RTLD_LOCAL);
  if (!handle) return 0;
  fun= (creationfunptr)dlsym(handle, creation_func.c_str());
  if (!fun)
  {
    dlclose(handle);
    return 0;
  }
  res=(*fun)(user_data);
  if (!res)
  {
    dlclose(handle);
    return 0;
  }
  res->systemhandle=handle;
#endif
  return res;
}

std::string SearchDll(std::string const& name)
{
  return name;
}

void UnloadDllObject(DllObjectBase* o)
{
  void *systemhandle= o->systemhandle;
  o->destroy();
#ifdef WIN32
  FreeLibrary((HMODULE)systemhandle);
#else
  dlclose(systemhandle);
#endif
}

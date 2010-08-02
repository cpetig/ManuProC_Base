/* $Id: EntryValueBase.h,v 1.15 2004/01/20 08:04:44 christof Exp $ */
/*  libcommonc++: ManuProC's main OO library
 *  Copyright (C) 2009-2010 Christof Petig
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

#ifndef ENTRY_VALUEEASY_H
#define ENTRY_VALUEEASY_H
#include <Misc/EntryValueBase.h>

namespace EntryValue_easy
{
  cH_EntryValue create(int const& a);
  cH_EntryValue create(double const& a);
  cH_EntryValue create(std::string const& a);
  cH_EntryValue create(bool const& a);
  cH_EntryValue create(cH_EntryValue const& a) { return a; }
  // dummy structure, used by RowDataBase_easy as default template argument
  struct nil {};
  cH_EntryValue create(nil const&);
  // generic pointer: ignore
  template <class T>
   cH_EntryValue create(T *const& a) { return cH_EntryValue(); }
};
#endif

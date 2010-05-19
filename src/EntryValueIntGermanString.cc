/* $Id: EntryValueIntGermanString.cc,v 1.1 2002/12/05 10:07:15 thoma Exp $ */
/*  libcommonc++: ManuProC's main OO library
 *  Copyright (C) 2000-2001 Adolf Petig GmbH & Co. KG, written by Christof Petig
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

#include <EntryValueIntGermanString.h>
#include <typeinfo> // for bad_cast
#include <Misc/germanstring.h>


bool EntryValueIntGermanString::operator<(const EntryValueBase &v) const
{
  try{
    const EntryValueIntGermanString &b=dynamic_cast<const EntryValueIntGermanString &>(v);  
    if(intval!=int_NaN) return intval < b.intval;
    return germanstring(strval) < germanstring(b.strval);
  } catch (std::bad_cast &e)
  {  return false;
  }
         
}

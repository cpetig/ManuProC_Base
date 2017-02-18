/* $Id: EntryValueDatum.cc,v 1.9 2003/10/09 12:37:51 jacek Exp $ */
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

#include <Misc/EntryValueDouble.h>
#include <typeinfo> // for bad_cast
#include <Misc/itos.h>

EntryValueDouble::EntryValueDouble(double d)
	: val(d), intval(int(d))
{
  strval=dtos(d);
}

bool EntryValueDouble::operator==(const EntryValueBase &v) const
{  try
   {  const EntryValueDouble &b=dynamic_cast<const EntryValueDouble &>(v);
      return val==b.val;
   } catch (std::bad_cast &e)
   {  return false;
   }
}

bool EntryValueDouble::operator<(const EntryValueBase &v) const
{  try
   {  const EntryValueDouble &b=dynamic_cast<const EntryValueDouble &>(v);
      return val<b.val;
   } catch (std::bad_cast &e)
   {  return false;
   }
}

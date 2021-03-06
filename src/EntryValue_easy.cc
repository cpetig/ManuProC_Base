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

#include <Misc/EntryValue_easy.h>
#include <Misc/EntryValueIntString.h>
#include <Misc/EntryValueDouble.h>
#include <Misc/EntryValueBool.h>
#include <Misc/create_parse.h>

cH_EntryValue EntryValue_easy::create(int const& a)
{ return cH_EntryValueIntString(a);
}

cH_EntryValue EntryValue_easy::create(bool const& a)
{ return cH_EntryValueBool(a);
}

cH_EntryValue EntryValue_easy::create(double const& a)
{ return cH_EntryValue(new EntryValueDouble(a));
}

cH_EntryValue EntryValue_easy::create(std::string const& a)
{ return cH_EntryValueIntString(a);
}

cH_EntryValue EntryValue_easy::create(nil const&)
{ return cH_EntryValue();
}

cH_EntryValue EntryValue_easy::create(cH_EntryValue const& a) { return a; }

template <> EntryValue_easy::nil ManuProC::parse<EntryValue_easy::nil>(const std::string &value)
{ return EntryValue_easy::nil();
}

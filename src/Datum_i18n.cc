// $Id: Datum.cc,v 1.40 2006/06/26 07:53:01 christof Exp $
/*  libcommonc++: ManuProC's main OO library
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

/* $Id: Datum.cc,v 1.40 2006/06/26 07:53:01 christof Exp $ */
#include "Datum.h"
#include <ManuProCConfig.h>
#include <Misc/string0.h>
#include <i18n.h>

void ManuProC::Datum::write_i18n(char *buf,unsigned int size) const throw(ManuProC::Datumsfehler)
{	teste();
	snprintf0(buf,size,"%d %s %04d",tag,dgettext(GETTEXT_PACKAGE, month_abbrev[monat-1]),jahr);
}

// not thread safe (at least)
const char *ManuProC::Datum::c_str() const throw(ManuProC::Datumsfehler)
{	static char ret[32]; // russian will use a large amount of bytes ;-)
	write_i18n(ret,sizeof ret);
	return ret;
}

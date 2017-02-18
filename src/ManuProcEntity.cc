/* $Id: ManuProcEntity.cc,v 1.4 2005/06/17 16:12:01 christof Exp $ */
/*  libcommonc++: ManuProC's main OO library
 *  Copyright (C) 2002 Adolf Petig GmbH & Co. KG
 *  written by Jacek Jakubowski, Christof Petig, Malte Thoma
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

#include <BaseObjects/ManuProcEintrag.h>

#if 0
bool ManuProcEntity::Valid(ID i) //const
{
   if(i==none_id) return false;
   return true;
}
#endif

#if __GNUC__ <3 || (__GNUC__==3 && __GNUC_MINOR__<4)
const ManuProcEntity<>::ID ManuProcEntity<>::none_id;
#elif (__GNUC__==4 && __GNUC_MINOR__>=2)
// I give up with newer compilers, perhaps this is a bug?
extern "C" const long _ZN14ManuProcEntityIlE7none_idE=-1;
extern "C" const long _ZN14ManuProcEntityIN8ManuProC12DynamicEnums13Kundengruppen6enum_tEE7none_idE=-1;
#else
template<> const ManuProcEntity<>::ID ManuProcEntity<>::none_id;
#endif


const ManuProcEintrag::ID ManuProcEintrag::none_znr;

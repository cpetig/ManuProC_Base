// $Id$
/*  libcommonc++: ManuProC's main OO library
 *  Copyright (C) 2009 Christof Petig
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "pg_type.h"
#include <Misc/itos.h>

std::string readable_SQLtype(int typ, int typmod)
{
  switch (typ)
  {
    case TEXTOID: return "text";
    case VARCHAROID: if (typmod==-1) return "varchar";
      return "varchar("+itos(typmod)+")";
    case INT4OID: return "integer";
    case BOOLOID: return "boolean";
    case NUMERICOID: if (typmod==-1) return "numeric";
      return "numeric("+itos(typmod>>16)+", "+itos((typmod&0xffff)-4)+")";
    case BPCHAROID: return "character("+itos(typmod-4)+")";
    case INT2OID: return "smallint";
    case FLOAT8OID: return "double precision";
    case FLOAT4OID: return "real";
  }
  return  itos(typ)+"/"+itos(typmod);
}

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

#include "Global_Settings.h"
#include <Misc/Query.h>

std::string Global_Settings::database_load(int userid,const std::string& program,const std::string& name)
{std::string wert;
 try
 {Query("select wert from global_settings "
#ifdef MPC_SQLITE
 	"where userid=? and program=? and name=?"
#else
 	"where (userid,program,name)=(?,?,?)"
#endif
 	)
   << userid << program << name
   >> wert;
 } catch (SQLerror &e)
 {
   if (e.Code()!=100)
   {
     e.print(__FILELINE__);
   }
   else try
   {
     Query("select wert from global_settings where userid is null and program=? and name=?")
	 << program << name
	 >> wert;
   }
   catch (SQLerror &f)
   {
     e.print(__FILELINE__,100);
   }
 }
 return wert;
}


void Global_Settings::database_save(int userid,const std::string& program,
                             const std::string& name,const std::string& wert)
{  // saves one query in comparison to Global_Settings().set_Wert
   //  ... the select ...
   Query *qp = NULL;
   if (userid==global_id)
   { qp= new Query("update global_settings set wert=? "
 	"where userid is null and program=? and name=?");
     (*qp) << wert << program << name;
   }
   else
   {
     qp = new Query("update global_settings set wert=? "
#ifdef MPC_SQLITE
 	"where userid=? and program=? and name=?"
#else
	"where (userid,program,name)=(?,?,?)"
#endif
	);
     (*qp) << wert << userid << program << name;
   }
  if (qp->Result()==100)
  {  Query("insert into global_settings "
	"(userid,program,name,wert) values (?,?,?,?)")
	<< Query::NullIf(userid,global_id) << program << name << wert;
     SQLerror::test(__FILELINE__);
  }
  delete qp;
}

void Global_Settings::database_connected(bool on)
{  if (!on) set_impl(&default_load,&default_save);
   else set_impl(&database_load,&database_save);
}

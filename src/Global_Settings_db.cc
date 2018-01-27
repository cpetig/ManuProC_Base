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

const int Global_Settings::global_id; // this id is for setting and requesting the default value for all users

std::string Global_Settings::database_load(int userid, const std::string& program, const std::string& name)
{
  std::string wert;
  try
  {
    Query("select wert from global_settings where coalesce(userid,0)=? and program=? and name=?")
      << (userid==global_id ? 0 : userid) << program << name >> wert;
  } 
  catch (SQLerror &e)
  {
     e.print(__FILELINE__);
  }

  return wert;
}


void Global_Settings::database_save(int userid, const std::string& program, const std::string& name, const std::string& wert)
{
  Query *qp = new Query("update global_settings set wert=? where coalesce(userid,0)=? and program=? and name=?");
         
  // for pq LinesAffected is without function, we must detect the affected lines by dummy fetch
  if(qp->getDBType()==ManuProC::Connection::C_PostgreSQL)
  {
    delete qp;
    qp = new Query("update global_settings set wert=? where coalesce(userid,0)=? "
                   "and program=? and name=? returning userid"); 
  }

  try
  {
    (*qp) << wert << (userid==global_id ? 0 : userid) << program << name;

    if(qp->getDBType()==ManuProC::Connection::C_PostgreSQL)
    {
      Query::Row fi=qp->Fetch();
      if(!fi.good()) // PQ insert
      {
        Query("insert into global_settings (userid,program,name,wert) values (?,?,?,?)")
           << (userid==global_id ? 0 : userid) << program << name << wert;
      }
    }
    else if(qp->LinesAffected()<1) // SQLite insert
    {
      Query("insert into global_settings (userid,program,name,wert) values (?,?,?,?)")
           << (userid==global_id ? 0 : userid) << program << name << wert;
    }
  }
  catch(SQLerror &e)
  {
    e.print(__FILELINE__);
  }
  
  delete qp;
}

void Global_Settings::database_connected(bool on)
{  if (!on) set_impl(&default_load,&default_save);
   else set_impl(&database_load,&database_save);
}

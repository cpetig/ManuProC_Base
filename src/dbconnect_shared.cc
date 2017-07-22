// $Id: dbconnect_shared.cc,v 1.9 2005/03/30 12:32:58 jacek Exp $
/*  libcommonc++: ManuProC's main OO library
 *  Copyright (C) 1998-2000 Adolf Petig GmbH & Co. KG, written by Jacek Jakubowski
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

#include <ManuProCConfig.h>
#include <Misc/dbconnect.h>
#include <cassert>
//#include <cstring>
//#include <unistd.h>
#include <cstdlib>
#include <Misc/Global_Settings.h>

#if defined(WIN32) && !defined(__MINGW32__)
#       pragma warning( disable : 4290 )  // for VS2010
#endif

ManuProC::Connection::Connection(const std::string &h, const std::string &d,
		const std::string &u,const std::string &n, const int p)
        	: host(h), dbase(d), user(u), name(n), port(p), type(C_SQLite)
{
   std::string host_=DEFAULT_DBHOST;
   std::string db_=DEFAULT_DB;
   std::string opt_port;

   char *h_opt(getenv("PGHOST"));
   char *d_opt(getenv("PGDATABASE"));
   char *p_opt(getenv("PGPORT"));
   char *t_opt(getenv("PGTYPE"));
   if(h_opt) host_=h_opt;
   if(d_opt) db_=d_opt;
   if(p_opt) opt_port=p_opt;
   if(t_opt && *t_opt)
   {
	  if (t_opt[0]=='P' || t_opt[0]=='p') type= ManuProC::Connection::C_PostgreSQL;
	  else if (t_opt[0]=='S' || t_opt[0]=='s') type= ManuProC::Connection::C_SQLite;
	  else if (isdigit(t_opt[0])) type= ManuProC::Connection::CType_t(strtol(t_opt,0,0));
   }
   if(host.empty())  host=host_;
   if(dbase.empty()) dbase=db_;
   if(!opt_port.empty()) port=atoi(opt_port.c_str());
}

std::string ManuProC::Connection::get_current_dbname()
{ 
	return Name();
}

Handle<ManuProC::Connection_base> ManuProC::dbconnect_nt(const Connection &c) throw()
{  try { return dbconnect(c); }
   catch (SQLerror &e)
   {  std::cerr << e << '\n';
   }
}

void ManuProC::dbdisconnect_nt(const std::string &name) throw()
{  try { dbdisconnect(name); }
   catch (SQLerror &e)
   {  std::cerr << e << '\n';
   }
}

Handle<ManuProC::Connection_base> ManuProC::dbconnect(const Connection &c) throw(SQLerror)
{
	Handle<Connection_base> res;
	switch(c.Type())
	{
	case Connection::C_PostgreSQL:
		res= dbconnect_PQ(c);
		break;
	case Connection::C_ECPG:
		res= dbconnect_ECPG(c);
		break;
	case Connection::C_SQLite:
		res= dbconnect_SQLite3(c);
		break;
	default:
		throw SQLerror("dbconnect", 100, "Database type unknown");
	}
	register_db(res);
	Global_Settings::database_connected();
	return res;
}

//#include <iostream>
//#include <fstream>

const std::string ManuProC::Connection::Pass() const throw(AuthError)
{
// not nedeed at the time

/* char buf[80];
 buf[0]=0;

  ifstream passfile (".remote_access");
  if (! passfile.is_open())
    { throw AuthError(".remote_access not found"); }
  if(!passfile.eof())
    passfile.getline(buf,sizeof buf);

  passfile.close();
*/
  return std::string();
}

void ManuProC::dbdisconnect(const std::string &name) throw(SQLerror)
{
	Global_Settings::database_connected(false);
	Handle<Connection_base> h=get_database(name);
	if(!h) return;
	unregister_db(h);
	h->disconnect();
}

void ManuProC::Connection_base::make_default() const throw()
{
	active_connection=const_cast<ManuProC::Connection_base*>(this);
}

Handle<ManuProC::Connection_base> ManuProC::dbdefault(std::string const& name) throw(SQLerror)
{
	active_connection= get_database(name);
	return active_connection;
}

std::vector<Handle<ManuProC::Connection_base> > ManuProC::connections;
Handle<ManuProC::Connection_base> ManuProC::active_connection;

void ManuProC::register_db(Handle<Connection_base> const& c)
{
	connections.push_back(c);
	if (!active_connection) active_connection=c;
}

void ManuProC::unregister_db(Handle<Connection_base> const& c)
{
	if (&*active_connection==&*c)
		active_connection = Handle<Connection_base>();
	for (std::vector<Handle<ManuProC::Connection_base> >::iterator i=connections.begin();i!=connections.end();++i)
		if (&*c == &**i)
		{
			connections.erase(i);
			return;
		}
}

Handle<ManuProC::Connection_base> ManuProC::get_database(std::string const& name) throw(SQLerror)
{
	if (name.empty()) return active_connection;
	for (std::vector<Handle<ManuProC::Connection_base> >::const_iterator i=connections.begin();i!=connections.end();++i)
		if ((*i)->Name()==name)
			return *i;
	throw SQLerror("get_database",100,"Database not found");
}

void ManuProC::Connection_base::setDTstyle(char const*) throw(SQLerror)
{

}

// until implemented
Handle<ManuProC::Connection_base> ManuProC::dbconnect_ECPG(ManuProC::Connection const&) throw(SQLerror)
{
	throw SQLerror("dbconnect_ECPG", 100, "not implemented");
}



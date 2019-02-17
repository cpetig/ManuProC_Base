// $Id: Transaction.pgcc,v 1.10 2004/04/29 08:38:47 christof Exp $
/*  libcommonc++: ManuProC's main OO library
 *  Copyright (C) 2008-2010 Christof Petig
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
#if 1 // def MPC_SQLITE  (now also used for pgSQL)
#include <Misc/Transaction.h>
#include <Misc/Query.h>
#include <algorithm>
#include <cassert>

#define USE_SPEED_HACK

std::list<Handle<ManuProC::Connection_base> > Transaction::open_connections;

// two handles are identical if they point to the same target
bool operator==(Handle<ManuProC::Connection_base>::ContentType const& a, Handle<ManuProC::Connection_base>::ContentType const&b)
{
	return &a==&b;
}

Transaction::Transaction(Handle<ManuProC::Connection_base> const& con,bool open_now)
	: owner(false), connection(con)
{ if (open_now) open(connection);
}

Transaction::Transaction(const std::string &_connection,bool open_now)
	: owner(false), connection(ManuProC::get_database(_connection))
{ if (open_now) open(connection);
}

void Transaction::open(Handle<ManuProC::Connection_base> const&con)
{  assert(!owner); // crash and burn on recursion!
   if (!!con) connection=con;

#ifdef USE_SPEED_HACK
   if (std::find(open_connections.begin(),open_connections.end(),connection)!=
   		open_connections.end())
   {  owner=commit_vs_rollback=false;
      return;
   }
#endif

   connection->execute("BEGIN TRANSACTION");
   open_connections.push_back(connection);
   
   owner=true;
   commit_vs_rollback=false;
}

void Transaction::open(const std::string &_connection)
{
	if (!_connection.empty())
		open(ManuProC::get_database(_connection));
	else open(Handle<ManuProC::Connection_base>());
}

void Transaction::open_exclusive(Handle<ManuProC::Connection_base> const&con)
{  assert(!owner); // crash and burn on recursion!
   if (!!con) connection=con;

#ifdef USE_SPEED_HACK
   if (std::find(open_connections.begin(),open_connections.end(),connection)!=
   		open_connections.end())
   {  std::cerr << "open exclusive transaction within transaction\n";
      owner=commit_vs_rollback=false;
      return;
   }
#endif

   assert(!!connection);
   connection->execute("BEGIN EXCLUSIVE TRANSACTION");
   open_connections.push_back(connection);
   
   owner=true;
   commit_vs_rollback=false;
}

void Transaction::open_exclusive(const std::string &_connection)
{
	if (!_connection.empty())
		open_exclusive(ManuProC::get_database(_connection));
	else open_exclusive(Handle<ManuProC::Connection_base>());
}

void Transaction::close()
{  if (owner)
   {  assert(!!connection);
      if (commit_vs_rollback) connection->execute("commit transaction");
      else connection->execute("rollback transaction");
#ifdef USE_SPEED_HACK
      open_connections.remove(connection);
#endif
      owner=false;
   }
}

Transaction::~Transaction()
{  if (owner) 
      try { close(); }
      catch (SQLerror &e) { std::cerr << e << '\n'; }
}
#endif

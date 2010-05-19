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
#ifdef MPC_SQLITE
#include <Misc/Transaction.h>
#include <Misc/Query.h>
#include <algorithm>
#include <cassert>

#define USE_SPEED_HACK

std::list<std::string> Transaction::open_connections;

Transaction::Transaction(const std::string &_connection,bool open_now) throw(SQLerror)
	: owner(false), connection(_connection)
{ if (open_now) open(connection);
}

void Transaction::open(const std::string &_connection) throw(SQLerror)
{  assert(!owner); // crash and burn on recursion!
   if (!_connection.empty()) connection=_connection;

#ifdef USE_SPEED_HACK
   if (std::find(open_connections.begin(),open_connections.end(),connection)!=
   		open_connections.end())
   {  owner=commit_vs_rollback=false;
      return;
   }
#endif

   assert(connection.empty());
   Query::Execute("BEGIN TRANSACTION");
   open_connections.push_back(connection);
   
   owner=true;
   commit_vs_rollback=false;
}

void Transaction::close() throw(SQLerror)
{  if (owner)
   {  assert(connection.empty());
      if (commit_vs_rollback) Query::Execute("commit transaction");
      else Query::Execute("rollback transaction");
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

// $Id: Event.cc,v 1.9 2006/06/26 07:53:01 christof Exp $
/*  libcommonc++: ManuProC's main OO library
 *  Copyright (C) 2003 Adolf Petig GmbH & Co. KG, written by Christof Petig
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

#include <Misc/Event.h>

//#ifdef HAVE_LIBECPG6_UP
//#define ECPGget_connection ecpg_get_connection
//#endif

ManuProC::Event::fullsignal_t ManuProC::Event::event_sig;
bool ManuProC::Event::connected;
ManuProC::TimeStamp ManuProC::Event::last_processed;
void *ManuProC::Event::PGconnection;

safemap<std::string, ManuProC::Event::filteredsignal_t> ManuProC::Event::channels;

#if 0
sigc::connection ManuProC::Event::connect(const std::string &channel,
   		const sigc::slot<void,const std::string &,const std::string &> &slot)
{  return signal_event(channel).connect(slot); }
#endif

#ifdef MPC_SQLITE
bool ManuProC::Event::look_for_notifications() { return false; }
void ManuProC::Event::read_notifications() {}
ManuProC::Event::Event(const std::string &channel,const std::string &key,const std::string &data) {}
void ManuProC::Event::connect(bool ignore_old) {}
int ManuProC::Event::filedesc() { return -1; }

#elif defined(MANUPROC_WITH_DATABASE)
#include <Misc/Query.h>
#include <libpq-fe.h>
#include <Misc/Transaction.h>

bool ManuProC::Event::look_for_notifications()
{  bool consumed=false;
retry:
   PQconsumeInput((PGconn *)PGconnection);
   PGnotify *n;
   bool pending=false;
   while ((n=PQnotifies((PGconn *)PGconnection)))
   {  if (n->relname==std::string("events")) pending=true;
   }
   if (pending) 
   {  consumed=true;
      read_notifications();
      goto retry;
   }
   return consumed;
}
	
void ManuProC::Event::read_notifications()
{  Transaction tr;
   ManuProC::TimeStamp bis;
   Query("select max(zeit) from events") >> bis;
   if (!bis.valid()) return;
   if (!last_processed.valid()) last_processed=ManuProC::TimeStamp("1970-1-1");
   Query q("SELECT channel,key,data FROM events "
   	"WHERE ? < zeit and zeit <= ? ORDER BY zeit");
   q << last_processed << bis;
   Query::Row is;
   while ((q >> is).good())
   {  std::string channel,key,data;
      is >> channel 
      	 >> Query::Row::MapNull(key)
      	 >> Query::Row::MapNull(data);
      event_sig(channel,key,data);
      channels[channel](key,data);
   }
   tr.commit();
   last_processed=bis;
}

ManuProC::Event::Event(const std::string &channel,const std::string &key,const std::string &data)
{  Query("INSERT INTO events (zeit,channel,key,data) VALUES (now(),?,?,?)")
	<< channel << Query::NullIf(key) << Query::NullIf(data);
   Query("NOTIFY events");
   Query("DELETE FROM events WHERE zeit+'10min'<now()");
}

namespace {
#ifndef HAVE_LIBECPG6_UP
struct ecpg_connection // stolen from ecpg internals
{   char	   *name;
    PGconn	   *connection;
    // ...
};

extern "C" ecpg_connection *ECPGget_connection(const char *);
#else
extern "C" PGconn* ECPGget_PGconn(const char *connection_name);
#endif
}

void ManuProC::Event::connect(bool ignore_old)
{  Query("LISTEN events");
   connected=true;
#ifdef HAVE_LIBECPG6_UP
   PGconnection=ECPGget_PGconn(0);
#else
   PGconnection=ECPGget_connection(0)->connection;
#endif
   if (ignore_old)
   {  Query("SELECT now()") >> last_processed;
      while (PQnotifies((PGconn *)PGconnection));
   }
   else read_notifications();
}

int ManuProC::Event::filedesc()
{  if (!connected) return -1;
   return PQsocket((PGconn *)PGconnection);
}

#endif

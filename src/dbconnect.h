// $Id: dbconnect.h,v 1.16 2005/11/22 13:04:34 christof Exp $
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

#include <Misc/SQLerror.h>
#include <Misc/Handles.h>
#ifdef MPC_SQLITE
struct sqlite3;
#endif

#define POSTGRESQL_PORT	5432

namespace ManuProC
{

class AuthError : public std::exception
{
 std::string msg;

public:

 ~AuthError() throw() {}
 AuthError(const std::string &m) throw() :msg(m) {}
 const std::string Msg() const { return msg; }

};

#if 0 //def MPC_SQLITE
	extern sqlite3 *db_connection;
#endif
   class Connection // connect options
   {
    std::string host;
    std::string dbase;
    std::string user;
    std::string name;
    int port;

    public:
     Connection(const std::string &h=std::string(), const std::string &d=std::string(),
                const std::string &u=std::string(),const std::string &n=std::string(),
                const int p=POSTGRESQL_PORT);


    const std::string Host() const { return host; }
    const std::string Dbase() const { return dbase; }
    const std::string User() const { return user; }
    const std::string Name() const { return name; }
    const std::string Pass() const throw(AuthError);
    const int Port() const { return port; }
    void setHost(const std::string &h) { if(!h.empty()) host=h; }
    void setDbase(const std::string &d) { if(!d.empty()) dbase=d; }
    void setUser(const std::string &u) { user=u; }
    void setName(const std::string &n) { name=n; }
    void setPort(const int p) { port=p;}
    // alternative API:
    void Host(const std::string &h) { host=h; }
    void Dbase(const std::string &d) { dbase=d; }
    void User(const std::string &u) { user=u; }
    void Name(const std::string &n) { name=n; }
    void Port(const int p) { port=p; }
   };

   class Connection_base : public HandleContent // actual connection object
   {
/*     transaction
     query execution
     error testing */
     virtual void open_transaction() throw(SQLerror)=0;
     virtual void commit_transaction() throw(SQLerror)=0;
     virtual void rollback_transaction() throw(SQLerror)=0;
     virtual void make_default() throw(SQLerror) {}
     virtual void setDTstyle(char const* style="ISO") throw(SQLerror);
     //virtual Query_base
   };

   std::vector<Handle<Connection_base> > connections;
   Handle<Connection_base> active_connection;

   Connection_base& dbconnect_nt(const Connection &c=Connection()) throw();
   void dbdisconnect_nt(const std::string &name=std::string()) throw();
   Connection_base& dbconnect(const Connection &c=Connection()) throw(SQLerror);
   void dbdisconnect(const std::string &name=std::string()) throw(SQLerror);
   void dbdisconnect_nt(Connection_base&) throw();
   void dbdisconnect(Connection_base&) throw(SQLerror);
   void setDTstyle(char const*style="ISO") throw(SQLerror);
   Connection_base& dbdefault(const std::string &name=std::string()) throw(SQLerror);
   void dbdefault(Connection_base&) throw(SQLerror);
   std::string get_dbname();
   std::string get_dbname(Connection_base&);
   Connection_base& get_database(std::string const& name);
   void register_db(Handle<Connection_base> const& c);
};

namespace Petig
{  using namespace ManuProC;
};

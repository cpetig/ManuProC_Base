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

#ifndef MPB_DBCONNECT_H
#define MPB_DBCONNECT_H

#if defined(WIN32) && !defined(__MINGW32__)
#	pragma once
#	pragma warning( disable : 4290 )  // for VS2010
#endif

#include <Misc/SQLerror.h>
#include <Misc/Handles.h>
#ifdef MPC_SQLITE
struct sqlite3;
#endif
#include <vector>


#define POSTGRESQL_PORT	5432

namespace ManuProC
{

// authentication error (?) used by Connection::Pass()
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
   class Connection_base;
   class Connection // connect options
   {
   public:
	enum CType_t { C_PostgreSQL /*libpq*/, C_ECPG, C_SQLite, C_virtual /* aka Fake */, C_default };

   private:
    std::string host;
    std::string dbase;
    std::string user;
    std::string name;
    int port;
    CType_t type;

    //friend class Connection_base;

    public:
     Connection(const std::string &h=std::string(), const std::string &d=std::string(),
                const std::string &u=std::string(),const std::string &n=std::string(),
                const int p=POSTGRESQL_PORT);


    const std::string Host() const { return host; }
    const std::string Dbase() const { return dbase; }
    const std::string User() const { return user; }
    const std::string Name() const { return name; }
    const std::string Pass() const throw(AuthError);
    int Port() const { return port; }
    CType_t Type() const { return type; }
    void setHost(const std::string &h) { if(!h.empty()) host=h; }
    void setDbase(const std::string &d) { if(!d.empty()) dbase=d; }
    void setUser(const std::string &u) { user=u; }
    void setName(const std::string &n) { name=n; }
    void setPort(const int p) { port=p;}
    void setType(CType_t t) { type=t; }
    // alternative API:
    void Host(const std::string &h) { host=h; }
    void Dbase(const std::string &d) { dbase=d; }
    void User(const std::string &u) { user=u; }
    void Name(const std::string &n) { name=n; }
    void Port(const int p) { port=p; }

    std::string get_current_dbname();
    //std::string get_dbname(Connection_base&);
   };

   typedef unsigned long Oid;

   struct Query_result_row
   {
	   virtual unsigned columns() const throw()=0;
	   virtual int indicator(unsigned col) const=0;
	   virtual char const* text(unsigned col) const=0;
	   // Oid?
//	   virtual double number(unsigned col) const=0;
//	   virtual long integer(unsigned col) const=0;
	   virtual char const* getFieldName(unsigned col) const { return "?"; }
   };

   struct Query_result_base
   {
	   virtual ~Query_result_base() {}
	   virtual unsigned LinesAffected() const throw()=0;
	   virtual Query_result_row* Fetch()=0;
	   virtual void AddParameter(const std::string &s, Oid type)=0;
	   virtual void AddNull(Oid type=0)=0;
//	   virtual void AddParameter(long integer, Oid type)=0;
//	   virtual void AddParameter(double fl, Oid type)=0;
	   virtual bool complete() const throw()=0;
	   virtual int last_insert_rowid() const { return -1; }
   };

   struct Prepared_Statement_base
   {
	   // execution is delayed until last parameter is passed
	   virtual Query_result_base* execute() throw(SQLerror)=0;
	   virtual ~Prepared_Statement_base() {}
	   /// check prepared statement against this connection (might re-prepare), true indicates new connection
	   virtual bool check_connection(Connection_base const&) {}
   };

   class Connection_base : public HandleContent // actual connection object
   {
/*     transaction
     query execution
     error testing */
   public:
#if 0 // in all databases identical to execute("BEGIN") etc
     virtual void open_transaction() throw(SQLerror)=0;
     virtual void commit_transaction() throw(SQLerror)=0;
     virtual void rollback_transaction() throw(SQLerror)=0;
#endif
     virtual void make_default() const throw();
     virtual void setDTstyle(char const* style="ISO") throw(SQLerror);
     //virtual Query_base
     virtual void disconnect() throw()=0;
     virtual std::string const& Name() const throw()=0;
     virtual Connection::CType_t Type() const throw()=0;
 	 virtual void execute(char const*) throw(SQLerror)=0;
 	 // with results
 	 virtual Query_result_base* execute2(char const*) throw(SQLerror)=0;
 	 // execution is delayed until last parameter is passed
 	 virtual Query_result_base* execute_param(char const* q, unsigned num) throw(SQLerror)=0;

 	 // Cursors (fetch a few lines at a time)
 	 virtual Query_result_base* open_cursor(char const* name, char const* q, unsigned num) throw(SQLerror)=0;

 	 virtual Prepared_Statement_base* prepare(char const* name, char const* q, unsigned numparam, ManuProC::Oid const* types) throw(SQLerror)=0;
// 	 virtual Query_result_base* execute_prepared(char const* name, unsigned numparam) throw(SQLerror)=0;

 	 // prepared queries?
 	 virtual int LastError() const throw()=0;
   };

   extern std::vector<Handle<Connection_base> > connections;
   extern Handle<Connection_base> active_connection;

   Handle<Connection_base> dbconnect_nt(const Connection &c=Connection()) throw();
   void dbdisconnect_nt(const std::string &name=std::string()) throw();
   Handle<Connection_base> dbconnect(const Connection &c=Connection()) throw(SQLerror);
   void dbdisconnect(const std::string &name=std::string()) throw(SQLerror);
   void dbdisconnect_nt(Connection_base&) throw();
   void dbdisconnect(Connection_base&) throw(SQLerror);
   void setDTstyle(char const*style="ISO") throw(SQLerror);
   Handle<Connection_base> dbdefault(const std::string &name=std::string()) throw(SQLerror);
   void dbdefault(Connection_base&) throw(SQLerror);
   Handle<Connection_base> get_database(std::string const& name=std::string()) throw(SQLerror);
   void register_db(Handle<Connection_base> const& c);
   void unregister_db(Handle<Connection_base> const& c);

   // internal:
   Handle<Connection_base> dbconnect_SQLite3(const Connection &c) throw(SQLerror);
   Handle<Connection_base> dbconnect_ECPG(const Connection &c) throw(SQLerror);
   Handle<Connection_base> dbconnect_PQ(const Connection &c) throw(SQLerror);
};

namespace Petig
{  using namespace ManuProC;
};


#endif


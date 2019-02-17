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
#include <stdint.h>

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

typedef unsigned long Oid;

struct null_s
{ ManuProC::Oid type;
  null_s(ManuProC::Oid t) : type(t) {}
};

class ArgumentEntry
{
	typedef ManuProC::Oid Oid;
	Oid type;
	bool null;
	std::string s;
	int64_t i;
	double f;

public:
	ArgumentEntry() : type(), null(true), s(), i(), f() {}
	ArgumentEntry(std::string const &s2, Oid o) : type(o), null(), s(s2), i(), f() {}
	ArgumentEntry(null_s const& n) : type(n.type), null(true), s(), i(), f() {}

	ArgumentEntry(std::string const &s);
	ArgumentEntry(int64_t a);
	ArgumentEntry(int32_t a);
	ArgumentEntry(double f);
	ArgumentEntry(bool b);
	ArgumentEntry(char const *s);

	Oid get_type() const { return type; }
	bool get_null() const { return null; }
	std::string const& get_string() const { return s; }
	int64_t get_int() const { return i; }
	double get_float() const { return f; }
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
    std::string password;
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
    const char* Password() const;
    int Port() const { return port; }
    CType_t Type() const { return type; }
    void setHost(const std::string &h) { if(!h.empty()) host=h; }
    void setDbase(const std::string &d) { if(!d.empty()) dbase=d; }
    void setUser(const std::string &u) { user=u; }
    void setPassword(const std::string &p) { password=p; }    
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

   struct Query_result_row
   {
	   virtual unsigned columns() const throw()=0;
	   virtual int indicator(unsigned col) const=0;
	   virtual char const* text(unsigned col) const=0;
	   // Oid?
//	   virtual double number(unsigned col) const=0;
//	   virtual long integer(unsigned col) const=0;
	   virtual char const* getFieldName(unsigned) const { return "?"; }
   };

   struct Query_result_base
   {
	   virtual ~Query_result_base() {}
	   virtual unsigned LinesAffected() const throw()=0;
	   virtual Query_result_row* Fetch()=0;
	   virtual void AddParameter(ManuProC::ArgumentEntry const& a)=0;
	   virtual bool complete() const throw()=0;
	   virtual int last_insert_rowid() const { return -1; }
   };

   struct Prepared_Statement_base
   {
	   // execution is delayed until last parameter is passed
	   virtual Query_result_base* execute()=0;
	   virtual ~Prepared_Statement_base() {}
	   /// check prepared statement against this connection (might re-prepare), true indicates new connection
	   virtual bool check_connection(Connection_base const&) { return true; }
   };

   class Connection_base : public HandleContent // actual connection object
   {
/*     transaction
     query execution
     error testing */
   public:
#if 0 // in all databases identical to execute("BEGIN") etc
     virtual void open_transaction()=0;
     virtual void commit_transaction()=0;
     virtual void rollback_transaction()=0;
#endif
     virtual void make_default() const throw();
     virtual void setDTstyle(char const* style="ISO");
     //virtual Query_base
     virtual void disconnect() throw()=0;
     virtual std::string const& Name() const throw()=0;
     virtual Connection::CType_t Type() const throw()=0;
 	 virtual void execute(char const*)=0;
 	 // with results
 	 virtual Query_result_base* execute2(char const*)=0;
 	 // execution is delayed until last parameter is passed
 	 virtual Query_result_base* execute_param(char const* q, unsigned num)=0;

 	 // Cursors (fetch a few lines at a time)
 	 virtual Query_result_base* open_cursor(char const* name, char const* q, unsigned num)=0;

 	 virtual Prepared_Statement_base* prepare(char const* name, char const* q, unsigned numparam, ManuProC::Oid const* types)=0;
// 	 virtual Query_result_base* execute_prepared(char const* name, unsigned numparam)=0;

 	 // prepared queries?
 	 virtual int LastError() const throw()=0;
   };

   extern std::vector<Handle<Connection_base> > connections;
   extern Handle<Connection_base> active_connection;

   Handle<Connection_base> dbconnect_nt(const Connection &c=Connection()) throw();
   void dbdisconnect_nt(const std::string &name=std::string()) throw();
   Handle<Connection_base> dbconnect(const Connection &c=Connection());
   void dbdisconnect(const std::string &name=std::string());
   void dbdisconnect_nt(Connection_base&) throw();
   void dbdisconnect(Connection_base&);
   void setDTstyle(char const*style="ISO");
   Handle<Connection_base> dbdefault(const std::string &name=std::string());
   void dbdefault(Connection_base&);
   Handle<Connection_base> get_database(std::string const& name=std::string());
   void register_db(Handle<Connection_base> const& c);
   void unregister_db(Handle<Connection_base> const& c);

   // internal:
   Handle<Connection_base> dbconnect_SQLite3(const Connection &c);
   Handle<Connection_base> dbconnect_ECPG(const Connection &c);
   Handle<Connection_base> dbconnect_PQ(const Connection &c);
};

namespace Petig
{  using namespace ManuProC;
};


#endif


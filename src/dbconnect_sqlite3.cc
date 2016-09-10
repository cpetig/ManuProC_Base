

#include <ManuProCConfig.h>
#include "dbconnect.h"

#ifdef MPC_SQLITE
#include <sqlite3.h>

struct sqliteConnection
{
	std::string name;
	sqlite3 *db_connection;

	virtual void open_transaction() throw(SQLerror);
	virtual void commit_transaction() throw(SQLerror);
	virtual void rollback_transaction() throw(SQLerror);
	virtual void setDTstyle(char const* style="ISO") throw(SQLerror)
	{}
	virtual void disconnect() throw();
	virtual std::string const& Name() throw() { return name; }
};

Handle<ManuProC::Connection_base> ManuProC::dbconnect_SQLite3(const Connection &c) throw(SQLerror)
{  //assert(!db_connection);

	sqlite3 *db_connection=0;
//   char *opt(getenv("SQLOPT"));
   int error=sqlite3_open(c.Dbase().c_str(),&db_connection);
   if (error)
   {  std::string err=sqlite3_errmsg(db_connection);
      sqlite3_close(db_connection);
      db_connection=0;
      throw(SQLerror("dbconnect",error,err));
   }
   // sollte nicht passieren ...

}


#else

Handle<ManuProC::Connection_base> ManuProC::dbconnect_SQLite3(const Connection &c) throw(SQLerror)
{
	throw SQLerror("Database type not implemented");
}
#endif

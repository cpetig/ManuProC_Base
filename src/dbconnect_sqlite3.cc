

#include <ManuProCConfig.h>
#include "dbconnect.h"

#ifdef MPC_SQLITE
#include <sqlite3.h>

struct sqliteConnection
{
	std::string name;
	sqlite3 *db_connection;
	int last_code;

//	virtual void open_transaction() throw(SQLerror);
//	virtual void commit_transaction() throw(SQLerror);
//	virtual void rollback_transaction() throw(SQLerror);
//	virtual void setDTstyle(char const* style="ISO") throw(SQLerror)
//	{}
	virtual void disconnect() throw();
	virtual std::string const& Name() throw() { return name; }
	virtual void execute(char const*) throw(SQLerror);
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
   sqliteConnection* res= new sqliteConnection;
   res->name= c.name;
   res->last_code= 0;
   res->db_connection=db_connection;
   return res;
}

void sqliteConnection::disconnect()
{
	sqlite3_close(db_connection);
	db_connection=NULL;
}

//void sqliteConnection::open_transaction() throw(SQLerror)
//{
//	execute("begin");
//}
//void sqliteConnection::commit_transaction() throw(SQLerror)
//{
//	execute("commit");
//}
//void sqliteConnection::rollback_transaction() throw(SQLerror)
//{
//	execute("rollback");
//}

void sqliteConnection::execute(char const* query) throw(SQLerror)
{
	//char **local_result=0;
	   char *msgbuf=0;
	//   int rows,cols;


	   if (Query::debugging.on) std::cerr << "QUERY: " << query << '\n';
	   error=sqlite3_exec(db_connection, query, 0, 0, &msgbuf);

/*	   error=sqlite3_get_table(db_connection, query.c_str(),
	   		&local_result, &rows, &cols, &msgbuf);*/
	   last_code=error;
	   if (Query::debugging.on)
	      std::cerr << "RESULT: " << error << ':' << (msgbuf?msgbuf:"")
	      		<< ", " << rows << 'x' << cols << '\n';
	   if(error!=SQLITE_OK)
	   {  std::string err=msgbuf;
	      sqlite3_free(msgbuf);
	      throw SQLerror(query,error,err);
	   }
	   /*lines=rows;
	   nfields=cols;*/
	   if (msgbuf) sqlite3_free(msgbuf);
/*	   if (!lines && strncasecmp(query.c_str(),"select",6))
	     lines=sqlite3_changes(ManuProC::db_connection);
	   if (!lines) SQLerror::last_code=error=100;
	   result=local_result;
	   eof=!lines; */
}

#else

Handle<ManuProC::Connection_base> ManuProC::dbconnect_SQLite3(const Connection &c) throw(SQLerror)
{
	throw SQLerror("Database type not implemented");
}
#endif

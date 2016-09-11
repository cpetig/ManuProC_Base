

#include <ManuProCConfig.h>
#include "dbconnect.h"

#ifdef MPC_SQLITE
#include <sqlite3.h>
#include <Misc/Query.h>
#include <string.h>

struct sqliteConnection : ManuProC::Connection_base
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
	virtual std::string const& Name() const throw() { return name; }
	virtual void execute(char const*) throw(SQLerror);
	virtual ManuProC::Connection::CType_t Type() const throw() { return ManuProC::Connection::C_SQLite; }
	virtual ManuProC::Query_result_base* execute2(char const*) throw(SQLerror);
	virtual int LastError() const throw() { return last_code; }
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
   res->name= c.Name();
   res->last_code= 0;
   res->db_connection=db_connection;
   return res;
}

void sqliteConnection::disconnect() throw()
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
	   int error=sqlite3_exec(db_connection, query, 0, 0, &msgbuf);

/*	   error=sqlite3_get_table(db_connection, query.c_str(),
	   		&local_result, &rows, &cols, &msgbuf);*/
	   last_code=error;
	   if (Query::debugging.on)
	      std::cerr << "RESULT: " << error << ':' << (msgbuf?msgbuf:"")
	      		/*<< ", " << rows << 'x' << cols */<< '\n';
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

struct rowSQ : ManuProC::Query_result_row
{
	char **local_result;
	int rows,cols;
	unsigned row;

	virtual unsigned columns() const throw() { return cols; }
	virtual int indicator(unsigned col) const { return local_result[row*cols+col]!=0; }
	virtual char const* text(unsigned col) const { return local_result[row*cols+col]; }
};

struct resultsSQ : ManuProC::Query_result_base
{
	sqliteConnection *conn;
	char **local_result;
	unsigned next_row;
	int rows,cols;
	int lines;
	rowSQ rowres;

	virtual unsigned LinesAffected() const throw() { return lines; }
	virtual ManuProC::Query_result_row* Fetch()
	{
		if (next_row>=rows)
		{
			conn->last_code=100;
			return 0;
		}
		rowres.local_result=local_result;
		rowres.row=next_row;
		rowres.rows=rows;
		rowres.cols=cols;
		++next_row;
		return &rowres;
	}
	virtual void AddParameter(const std::string &s, ManuProC::Oid type) {}
//	   virtual void AddParameter(long integer, Oid type)=0;
//	   virtual void AddParameter(double fl, Oid type)=0;
	virtual bool complete() const throw() { return true; }

	resultsSQ() : conn(), local_result(), next_row(), rows(),cols(),lines() {}
	~resultsSQ() { if (local_result) sqlite3_free_table(local_result); }
};

ManuProC::Query_result_base* sqliteConnection::execute2(char const* query) throw(SQLerror)
{
	char **local_result=0;
	   char *msgbuf=0;
	   int rows,cols;

	   if (Query::debugging.on) std::cerr << "QUERY: " << query << '\n';

	   int error=sqlite3_get_table(db_connection, query,
	   		&local_result, &rows, &cols, &msgbuf);
	   last_code=error;
	   if (Query::debugging.on)
	      std::cerr << "RESULT: " << error << ':' << (msgbuf?msgbuf:"")
	      		<< ", " << rows << 'x' << cols << '\n';
	   if(error!=SQLITE_OK)
	   {  std::string err=msgbuf;
	      sqlite3_free(msgbuf);
	      throw SQLerror(query,error,err);
	   }
	   resultsSQ* res= new resultsSQ;
	   res->rows= rows;
	   res->cols= cols;
	   res->local_result= local_result;
	   res->lines= rows;
	   if (msgbuf) sqlite3_free(msgbuf);
	   if (!res->lines && strncasecmp(query,"select",6))
	     res->lines=sqlite3_changes(db_connection);
	   if (!res->lines) last_code=error=100;
	   res->conn=this;
	   return res;
}

#else

Handle<ManuProC::Connection_base> ManuProC::dbconnect_SQLite3(const Connection &c) throw(SQLerror)
{
	throw SQLerror("dbconnect_SQLite3", 100, "Database type not implemented");
}
#endif

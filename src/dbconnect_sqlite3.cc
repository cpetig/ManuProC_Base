

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

	virtual void disconnect() throw();
	virtual std::string const& Name() const throw() { return name; }
	virtual void execute(char const*) throw(SQLerror);
	virtual ManuProC::Connection::CType_t Type() const throw() { return ManuProC::Connection::C_SQLite; }
	virtual ManuProC::Query_result_base* execute2(char const*) throw(SQLerror);
	virtual int LastError() const throw() { return last_code; }
	virtual ManuProC::Query_result_base* execute_param(char const* q, unsigned num) throw(SQLerror);
	virtual ManuProC::Prepared_Statement_base* prepare(char const* name, char const* q, unsigned numparam, ManuProC::Oid const* types) throw(SQLerror) { return 0; }
};

Handle<ManuProC::Connection_base> ManuProC::dbconnect_SQLite3(const Connection &c) throw(SQLerror)
{  sqlite3 *db_connection=0;
   if (Query::debugging.on) std::cerr << "SQLite3 connect: " << c.Name() << " " << c.Dbase() << '\n';

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

// TODO: Lines affected ?
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
	rowSQ() : local_result(), rows(), cols(), row() {}
};

struct rowSQ_step : ManuProC::Query_result_row
{
	sqlite3_stmt *stmt;

	virtual unsigned columns() const throw() { return sqlite3_column_count(stmt); }
	virtual int indicator(unsigned col) const { return sqlite3_column_type(stmt,col)==SQLITE_NULL ? -1 : 0; }
	virtual char const* text(unsigned col) const { return (const char*)sqlite3_column_text(stmt,col); }
	virtual char const* getFieldName(unsigned col) const { return sqlite3_column_database_name(stmt,col); }
	rowSQ_step() : stmt() {}
};

struct SQparam
{
	std::string s;
	bool null;
	SQparam(std::string const& a) : s(a), null() {}
	SQparam() : null(true) {}
};

struct resultsSQ_params : ManuProC::Query_result_base
{
	sqliteConnection *conn;
	std::string query;
	unsigned missing_params;
	std::vector<SQparam> parameters;
	bool ready;
	rowSQ_step step;
	unsigned lines;

	virtual unsigned LinesAffected() const throw() { return lines; }
	virtual ManuProC::Query_result_row* Fetch()
	{
		if (!ready) fetch();
		if (!ready) return 0;
		ready= false; // fetch again on next call
		return &step;
	}
	virtual void AddParameter(const std::string &s, ManuProC::Oid type);
	virtual void AddNull(ManuProC::Oid type);
//	   virtual void AddParameter(long integer, Oid type)=0;
//	   virtual void AddParameter(double fl, Oid type)=0;
	virtual bool complete() const throw() { return !missing_params; }
	virtual int last_insert_rowid() const
	{
		return sqlite3_last_insert_rowid(conn->db_connection);
	}
	void execute();
	void fetch();

	resultsSQ_params()
	: conn(), missing_params(), ready(), lines()
	{}
	~resultsSQ_params()
	{ if (step.stmt) sqlite3_finalize(step.stmt);
	}
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
	virtual void AddNull(ManuProC::Oid type) {}
//	   virtual void AddParameter(long integer, Oid type)=0;
//	   virtual void AddParameter(double fl, Oid type)=0;
	virtual bool complete() const throw() { return true; }
	virtual int last_insert_rowid() const
	{
		return sqlite3_last_insert_rowid(conn->db_connection);
	}

	resultsSQ()
	: conn(), local_result(), next_row(), rows(),cols(),lines()
	{}
	~resultsSQ()
	{ if (local_result) sqlite3_free_table(local_result);
	}
};

void resultsSQ_params::AddParameter(const std::string &s, ManuProC::Oid type)
{
	if (!missing_params) throw SQLerror(query,2,"too many parameters");
	parameters.push_back(SQparam(s));
	--missing_params;
	if (!missing_params) execute();
}

void resultsSQ_params::AddNull(ManuProC::Oid type)
{
	if (!missing_params) throw SQLerror(query,2,"too many parameters");
	parameters.push_back(SQparam());
	--missing_params;
	if (!missing_params) execute();
}

void resultsSQ_params::fetch()
{
	assert(step.stmt);
	int error = sqlite3_step(step.stmt);
	if (error!=SQLITE_DONE && error!=SQLITE_ROW)
	{
		missing_params=unsigned(-1);
//		sqlite3_reset(step.stmt);
		ready=false;
		throw(SQLerror(query,error,"step"));
	}
	if (error==SQLITE_DONE)
	{
		if (strncasecmp(query.c_str(),"select",6)) // we never expected lines as a result
		{
			conn->last_code=0;
		    lines= sqlite3_changes(conn->db_connection);
		    ready= true;
		}
		else
		{
			lines=0;
			ready=false;
			conn->last_code=100;
		}
	}
	else
	{
		conn->last_code=0;
		lines=1; // perhaps more!
		ready=true;

		printf("cols %d\n", sqlite3_column_count(step.stmt));
		for (unsigned i=0;i<sqlite3_column_count(step.stmt);++i)
			printf("type %d %d %s %s\n", i, sqlite3_column_type(step.stmt,i),
					sqlite3_column_database_name(step.stmt,i),
					sqlite3_column_text(step.stmt,i));
	}
}

void resultsSQ_params::execute()
{
	unsigned psize= parameters.size();

	int error= sqlite3_prepare_v2(conn->db_connection, query.c_str(), -1, &step.stmt, 0);
	if (error!=SQLITE_OK) throw SQLerror(query,error,"prepare");

	assert(sqlite3_bind_parameter_count(step.stmt)==psize);
	for (unsigned i=0;i<psize;++i)
	{
		if (parameters[i].null) sqlite3_bind_null(step.stmt,i+1);
		else sqlite3_bind_text(step.stmt, i+1, parameters[i].s.data(), parameters[i].s.size(), 0);
	}
	fetch();
}

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

ManuProC::Query_result_base* sqliteConnection::execute_param(char const* q, unsigned num) throw(SQLerror)
{
	if (!num) return execute2(q);

	resultsSQ_params* res2= new resultsSQ_params;
	res2->conn= this;
	res2->missing_params = num;
	res2->query= q;
	last_code=0;
	return res2;
}

#else

Handle<ManuProC::Connection_base> ManuProC::dbconnect_SQLite3(const Connection &c) throw(SQLerror)
{
	throw SQLerror("dbconnect_SQLite3", 100, "Database type not implemented");
}
#endif

#include "dbconnect.h"
#include <postgresql/libpq-fe.h>
#include <Misc/Query.h>

struct connectionPQ : ManuProC::Connection_base
{
	PGconn *connection;
	std::string name;
	int last_error;

	connectionPQ() : last_error(), connection() {}

//	virtual void open_transaction() throw(SQLerror);
//	virtual void commit_transaction() throw(SQLerror);
//	virtual void rollback_transaction() throw(SQLerror);
//	virtual void setDTstyle(char const* style="ISO") throw(SQLerror)
//	{}
	virtual void disconnect() throw();
	virtual std::string const& Name() const throw() { return name; }
	virtual void execute(char const*) throw(SQLerror);
	virtual ManuProC::Connection::CType_t Type() const throw() { return ManuProC::Connection::C_PostgreSQL; }
	virtual ManuProC::Query_result_base* execute2(char const*) throw(SQLerror);
	virtual int LastError() const throw() { return last_error; }
};

Handle<ManuProC::Connection_base> ManuProC::dbconnect_PQ(const Connection &c) throw(SQLerror)
{
	char pgport[20];
	snprintf(pgport,sizeof pgport, "%d", c.Port());
	const char *pghost= c.Host().c_str();
	const char *dbName= c.Dbase().c_str();
	const char *login= c.User().c_str();

	if (Query::debugging.on) std::cerr << "CONNECT: " << pghost << ':' << pgport << '\t' << dbName << '\n';

	PGconn * conn= PQsetdbLogin(pghost, pgport, 0, 0, dbName, login, 0);

	if (conn && PQstatus(conn) != CONNECTION_OK)
	{
		std::string msg= PQerrorMessage(conn);
		unsigned stat=PQstatus(conn);
		PQfinish(conn);
		if (Query::debugging.on) std::cerr << "Error: " << stat << '\t' << msg << '\n';
		throw(SQLerror("dbconnect_PQ",stat,msg));
	}

	if (conn)
	{
		connectionPQ* res= new connectionPQ;
		res->connection= conn;
		res->name= c.Name();
		return res;
	}
	else
	{
		if (Query::debugging.on) std::cerr << "Connection Error\n";
		throw(SQLerror("dbconnect_PQ",100,"not implemented"));
	}
}

void connectionPQ::disconnect() throw()
{
	PQfinish(connection);
	connection=NULL;
}

//void connectionPQ::open_transaction() throw(SQLerror)
//{
//	execute("begin");
//}
//void connectionPQ::commit_transaction() throw(SQLerror)
//{
//	execute("commit");
//}
//void connectionPQ::rollback_transaction() throw(SQLerror)
//{
//	execute("rollback");
//}

void connectionPQ::execute(char const* q) throw(SQLerror)
{
	if (Query::debugging.on) std::cerr << "QUERY: " << q << '\n';
	PGresult* res= PQexec(connection, q);
	if (PQresultStatus(res) != PGRES_COMMAND_OK)
	{
		std::string msg= PQresultErrorMessage(res);
		unsigned stat=PQresultStatus(res);
		PQclear(res);
		if (Query::debugging.on) std::cerr << "Error: " << stat << '\t' << msg << '\n';
		last_error= stat;
		throw(SQLerror(q,stat,msg));
	}
	if (Query::debugging.on) std::cerr << "OK\n";
	PQclear(res);
	last_error=0;
}

struct rowPQ : ManuProC::Query_result_row
{
	PGresult* res;
	unsigned row;

	// PQfname, PQftype

	virtual unsigned columns() const throw() { return PQnfields(res); }
	virtual int indicator(unsigned col) const { return PQgetisnull(res, row, col) ? -1 : 0; }
	virtual char const* text(unsigned col) const { return PQgetvalue(res, row, col); }
};

struct resultsPQ : ManuProC::Query_result_base
{
	connectionPQ *conn;
	PGresult* res;
	unsigned next_row;
	rowPQ rowres;

	virtual unsigned LinesAffected() const throw() { return PQntuples(res); }
	virtual ManuProC::Query_result_row* Fetch();
	virtual void AddParameter(const std::string &s, ManuProC::Oid type) {}
//	   virtual void AddParameter(long integer, Oid type)=0;
//	   virtual void AddParameter(double fl, Oid type)=0;
	virtual bool complete() const throw() { return true; }

	resultsPQ() : conn(), res(), next_row() {}
	~resultsPQ() { if (res) PQclear(res); }
};

ManuProC::Query_result_row* resultsPQ::Fetch()
{
	if (next_row>=LinesAffected())
	{
		conn->last_error=100;
		return 0;
	}
	rowres.res=res;
	rowres.row=next_row;
	++next_row;
	return &rowres;
}

ManuProC::Query_result_base* connectionPQ::execute2(char const* q) throw(SQLerror)
{
	if (Query::debugging.on) std::cerr << "QUERY: " << q << '\n';
	PGresult* res= PQexec(connection, q);
	if (PQresultStatus(res) == PGRES_EMPTY_QUERY)
	{
		std::string msg= PQresultErrorMessage(res);
		PQclear(res);
		if (Query::debugging.on) std::cerr << "Empty: " << 100 << '\t' << msg << '\n';
		last_error=100;
		return 0;
//		throw(SQLerror(q,100,msg));
	}
	if (PQresultStatus(res) != PGRES_TUPLES_OK)
	{
		std::string msg= PQresultErrorMessage(res);
		unsigned stat=PQresultStatus(res);
		PQclear(res);
		last_error=stat;
		if (Query::debugging.on) std::cerr << "Error: " << stat << '\t' << msg << '\n';
		throw(SQLerror(q,stat,msg));
	}
	if (Query::debugging.on) std::cerr << "Returned " << PQntuples(res) << " lines with " << PQnfields(res) << " columns\n";
	resultsPQ* res2= new resultsPQ;
	res2->res= res;
	res2->conn= this;
	last_error=0;
	return res2;
//	PQclear(res);
}

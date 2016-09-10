#include "dbconnect.h"
#include <postgresql/libpq-fe.h>
#include <Misc/Query.h>

struct connectionPQ : ManuProC::Connection_base
{
	PGconn *connection;
	std::string name;

//	virtual void open_transaction() throw(SQLerror);
//	virtual void commit_transaction() throw(SQLerror);
//	virtual void rollback_transaction() throw(SQLerror);
//	virtual void setDTstyle(char const* style="ISO") throw(SQLerror)
//	{}
	virtual void disconnect() throw();
	virtual std::string const& Name() throw() { return name; }
	virtual void execute(char const*) throw(SQLerror);
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
		throw(SQLerror(q,stat,msg));
	}
	if (Query::debugging.on) std::cerr << "OK\n";
	PQclear(res);
}

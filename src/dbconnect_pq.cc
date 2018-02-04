/*  libcommonc++: ManuProC's main OO library
 *  Copyright (C) 2016-2018 Christof Petig
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

#include "dbconnect.h"
#include <libpq-fe.h>
#include <Misc/Query.h>
#include <string.h>
#include <Misc/itos.h>
#include <pg_type.h>

struct connectionPQ : ManuProC::Connection_base
{
	PGconn *connection;
	std::string name;
	int last_error;

	connectionPQ() : last_error(), connection() {}

	virtual void disconnect() throw();
	virtual std::string const& Name() const throw() { return name; }
	virtual void execute(char const*) throw(SQLerror);
	virtual ManuProC::Connection::CType_t Type() const throw() { return ManuProC::Connection::C_PostgreSQL; }
	virtual ManuProC::Query_result_base* execute2(char const*) throw(SQLerror);
	virtual int LastError() const throw() { return last_error; }
	virtual ManuProC::Query_result_base* execute_param(char const* q, unsigned num) throw(SQLerror);
	virtual ManuProC::Prepared_Statement_base* prepare(char const* name, char const* q, unsigned numparam, ManuProC::Oid const* types) throw(SQLerror);
	virtual ManuProC::Query_result_base* open_cursor(char const* name, char const* q, unsigned num) throw(SQLerror);
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

static void remove_prepared_from_connection(connectionPQ const* conn);

void connectionPQ::disconnect() throw()
{
	if (Query::debugging.on) std::cerr << "PostgreSQL: disconnect\n";
	remove_prepared_from_connection(this);
	PQfinish(connection);
	connection=NULL;
}

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
	rowPQ() : res(), row() {}
	virtual unsigned columns() const throw() { return PQnfields(res); }
	virtual int indicator(unsigned col) const { return PQgetisnull(res, row, col) ? -1 : 0; }
	virtual char const* text(unsigned col) const { return PQgetvalue(res, row, col); }
};

struct PQ_Prepared_Statement;

#if 0
struct PQparam
{
	std::string s;
	ManuProC::Oid type;
	bool null;
	PQparam(std::string const& a, ManuProC::Oid t=0) : s(a), type(t), null() {}
	PQparam(ManuProC::Oid t=0) : type(t), null(true) {}
};
#endif

struct resultsPQ : ManuProC::Query_result_base
{
	connectionPQ *conn;
	PGresult* res;
	unsigned next_row;
	rowPQ rowres;
	PQ_Prepared_Statement *prep;
	unsigned missing_params;
	std::string query;
	std::vector<ManuProC::ArgumentEntry> parameters;
	unsigned lines;

	// lines returned (or lines affected if none returned)
	virtual unsigned LinesAffected() const throw() { return lines; }
	virtual ManuProC::Query_result_row* Fetch();
	virtual void AddParameter(ManuProC::ArgumentEntry const& a);
	virtual bool complete() const throw() { return !missing_params; }
	// on postgreSQL this one returns an Oid!
	virtual int last_insert_rowid() const { return PQoidValue(res); }
	void execute();

	resultsPQ() : conn(), res(), next_row(), prep(), missing_params(), lines() {}
	~resultsPQ() { if (res) PQclear(res); }
};

struct PQ_Prepared_Statement : ManuProC::Prepared_Statement_base
{
	std::string name;
	unsigned numparam;
	connectionPQ *conn;
//	resultsPQ obj;
	// necessary across connections
	std::string query;
	std::vector< ::Oid> parameters;

    // execution is delayed until last parameter is passed
	virtual ManuProC::Query_result_base* execute() throw(SQLerror);
	virtual ~PQ_Prepared_Statement();
	virtual bool check_connection(ManuProC::Connection_base const& cb);
};

void resultsPQ::execute()
{
	unsigned psize= parameters.size();
	char const *values[psize];
	int lengths[psize];
	int formats[psize];
	::Oid types[psize];
	std::string storage[psize];
	memset(formats,0,psize*sizeof(*formats));
	for (unsigned i=0;i<psize;++i)
	{
		if (parameters[i].get_null())
		{
			values[i]=0;
			lengths[i]=0;
		}
		else if (parameters[i].get_type()==INT8OID || parameters[i].get_type()==INT4OID)
		{
			storage[i] = ulltos(parameters[i].get_int());
			values[i]= storage[i].data();
			lengths[i]= storage[i].size();
		}
		else if (parameters[i].get_type()==FLOAT4OID)
		{
			storage[i] = dtos(parameters[i].get_float());
			values[i]= storage[i].data();
			lengths[i]= storage[i].size();
		}
		else if (parameters[i].get_type()==BOOLOID)
		{
			storage[i] = itos(parameters[i].get_int());
			values[i]= storage[i].data();
			lengths[i]= storage[i].size();
		}
		else if (parameters[i].get_type()==TEXTOID || !parameters[i].get_string().empty())
		{
			values[i]= parameters[i].get_string().data();
			lengths[i]= parameters[i].get_string().size();
		}
		types[i]= parameters[i].get_type();
	}
	if (Query::debugging.on)
	{
		if (prep)
		{
			fprintf(stderr,"Executing prepared %s (", prep->name.c_str());
		}
		else
		{
			fprintf(stderr,"Executing %s (", query.c_str());
		}
		for (unsigned i=0;i<psize;++i)
		{
			if (!values[i]) fprintf(stderr,"NULL,");
			else fprintf(stderr,"%s,", values[i]);
		}
		fprintf(stderr,")\n");
	}
	if (prep)
		res= PQexecPrepared(conn->connection, prep->name.c_str(), prep->numparam, values, lengths, formats, 0);
	else
		res= PQexecParams(conn->connection, query.c_str(), psize, types, values, lengths, formats, 0);
	if (!res)
	{
		if (Query::debugging.on) std::cerr << "PQexec failed: " << (prep? prep->name : query) << '\n';
		missing_params=unsigned(-1);
		conn->last_error=1;
		throw(SQLerror(prep? prep->name : query,1,"server error"));
	}
	if (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK)
	{
		missing_params=unsigned(-1);
		std::string msg= PQresultErrorMessage(res);
		unsigned stat=PQresultStatus(res);
		PQclear(res);
		res=0;
		conn->last_error=stat;
		if (Query::debugging.on) std::cerr << "Error: " << stat << '\t' << msg << '\n';
		throw(SQLerror(prep? prep->name : query,stat,msg));
	}
	conn->last_error= PQresultStatus(res) == PGRES_TUPLES_OK ? 0 : 100;
	if (PQresultStatus(res) == PGRES_TUPLES_OK) lines= PQntuples(res);
	else
	{
		char const *ntup = PQcmdTuples(res);
		if (ntup) lines= atoi(ntup);
		else lines=0;
	}
	if (Query::debugging.on)
	{
		if (PQresultStatus(res) == PGRES_TUPLES_OK) fprintf(stderr,"Query returned %d lines\n", lines);
		else fprintf(stderr,"Query modified %d rows (%s)\n", lines, PQcmdTuples(res));
	}
	next_row=0;
}

void resultsPQ::AddParameter(ManuProC::ArgumentEntry const& a)
{
	if (!missing_params) throw SQLerror(query,2,"too many parameters");
	parameters.push_back(a);
	--missing_params;
	if (!missing_params) execute();
}

ManuProC::Query_result_row* resultsPQ::Fetch()
{
	if (next_row>=LinesAffected())
	{
		conn->last_error=100;
		return 0;
	}
	if (Query::debugging.on)
	{
		fprintf(stderr,"PostgreSQL Result ");
		for (unsigned i=0;i<PQnfields(res);++i)
		{
			if (PQgetisnull(res, next_row, i)) fprintf(stderr,"NULL,");
			else fprintf(stderr,"%s,", PQgetvalue(res, next_row, i));
		}
		fprintf(stderr,"\n");
	}
	rowres.res=res;
	rowres.row=next_row;
	++next_row;
	return &rowres;
}

static char const* convert_parameters(std::string& parameter_style, char const* q)
{
	// convert question marks to postgreSQL's style
	if (strchr(q,'?'))
	{
		char const* in=q;
		char const* start=q;
		char quote=0;
		unsigned pnumber=0;
		for (;*in;)
		{
			char const* delim= strpbrk(in, "?'\"");
			if (!delim) break;
			switch(*delim)
			{
			case '"':
			case '\'':
				if (!quote) quote=*delim;
				else if (quote==*delim)
				{
					if (*delim=='\'' && delim[1]=='\'') // literal quote
						++delim; // skip two characters
					else quote=0;
				}
				else ; // just skip
				break;
			case '?':
				if (quote) ; // just skip
				else
				{
					++pnumber;
					parameter_style+= std::string(start,delim);
					start=delim+1;
					parameter_style+= '$';
					parameter_style+= itos(pnumber);
				}
			}
			in=delim+1;
		}
		if (pnumber)
		{
			parameter_style+= std::string(start);
			q= parameter_style.c_str();
		}
	}
	return q;
}

// this doesn't execute directly
ManuProC::Query_result_base* connectionPQ::execute_param(char const* q, unsigned num) throw(SQLerror)
{
	if (!num) return execute2(q);
	std::string parameter_style;
	q= convert_parameters(parameter_style, q);

	resultsPQ* res2= new resultsPQ;
	res2->conn= this;
	res2->missing_params = num;
	res2->query= q;
	last_error=0;
	return res2;
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
	if (PQresultStatus(res) != PGRES_TUPLES_OK
		&& PQresultStatus(res) != PGRES_COMMAND_OK)
	{
		std::string msg= PQresultErrorMessage(res);
		unsigned stat=PQresultStatus(res);
		PQclear(res);
		last_error=stat;
		if (Query::debugging.on) std::cerr << "Error: " << stat << '\t' << msg << '\n';
		throw(SQLerror(q,stat,msg));
	}
//	if (Query::debugging.on) std::cerr << "Returned " << PQntuples(res) << " lines with " << PQnfields(res) << " columns\n";
	resultsPQ* res2= new resultsPQ;
	res2->res= res;
	res2->conn= this;
	if (PQresultStatus(res) == PGRES_TUPLES_OK) res2->lines= PQntuples(res);
	else
	{
		char const *ntup = PQcmdTuples(res);
		if (ntup) res2->lines= atoi(ntup);
		else res2->lines=0;
	}
	if (Query::debugging.on)
	{
		if (PQresultStatus(res) == PGRES_TUPLES_OK) fprintf(stderr,"Query returned %d lines with %d columns\n", res2->lines, PQnfields(res));
		else fprintf(stderr,"Query modified %d rows (%s)\n", res2->lines, PQcmdTuples(res));
	}
	last_error=0;
	return res2;
//	PQclear(res);
}

std::map<std::string,PQ_Prepared_Statement*> prepared_stmts;

static void remove_prepared_from_connection(connectionPQ const* conn)
{
	for (std::map<std::string,PQ_Prepared_Statement*>::iterator i= prepared_stmts.begin(); i!= prepared_stmts.end(); ++i)
	{
		if (i->second->conn==conn)
			i->second->conn=NULL;
	}
}

PQ_Prepared_Statement::~PQ_Prepared_Statement()
{
	if (!name.empty() && conn)
	{
		std::string cmd= "DEALLOCATE "+name;
		conn->execute(cmd.c_str());
	}
	std::map<std::string,PQ_Prepared_Statement*>::iterator i= prepared_stmts.find(name);
	if (i!=prepared_stmts.end())
		prepared_stmts.erase(i);
}

bool PQ_Prepared_Statement::check_connection(ManuProC::Connection_base const& cb)
{
#if 1 // re-preparing prepared query in new connection
	if (!conn)
	{
		conn= dynamic_cast<connectionPQ*>(const_cast<ManuProC::Connection_base*>(&cb));
		fprintf(stderr,"Prepared statement across connections, restoring\n");
		PGresult *res2= PQprepare(conn->connection, name.c_str(), query.c_str(), parameters.size(), parameters.data());
		if (!res2)
		{
			if (Query::debugging.on) std::cerr << "PQprepare failed: " << name << '\n';
			throw(SQLerror(query,1,"server error"));
		}
		if (PQresultStatus(res2) != PGRES_COMMAND_OK)
		{
			std::string msg= PQresultErrorMessage(res2);
			unsigned stat=PQresultStatus(res2);
			PQclear(res2);
			conn->last_error=stat;
			if (Query::debugging.on) std::cerr << "Error: " << stat << '\t' << msg << '\n';
			throw(SQLerror(query,stat,msg));
		}
		PQclear(res2);
		if (Query::debugging.on)
		{
			fprintf(stderr,"Re-Prepared %s for %s\n", name.c_str(), query.c_str());
		}
		return true;
	}
#endif
	return false;
}

ManuProC::Query_result_base* PQ_Prepared_Statement::execute() throw(SQLerror)
{
	resultsPQ* obj = new resultsPQ;
	obj->conn= conn;
	obj->prep= this;
	obj->missing_params= numparam;
	obj->parameters.clear();
	if (!obj->missing_params)
		obj->execute();
//	if (obj.res)
//	{
//		PQclear(obj.res);
//		obj.res=0;
//	}
	return obj;
}

static unsigned preparation_index=0;

ManuProC::Prepared_Statement_base* connectionPQ::prepare(char const* name, char const* q, unsigned numparam, ManuProC::Oid const* types) throw(SQLerror)
{
	std::string stringcontainer;
	if (!name)
	{
		++preparation_index;
		stringcontainer="Prep"+itos(preparation_index);
		name=stringcontainer.c_str();
	}
	std::map<std::string,PQ_Prepared_Statement*>::iterator i= prepared_stmts.find(name);
	if (i!=prepared_stmts.end()) delete i->second;
	PQ_Prepared_Statement*& res= prepared_stmts[name];
	::Oid types2[numparam];
	if (types) for (unsigned i=0;i<numparam;++i) types2[i]=types[i];
	else memset(types2, 0, sizeof(*types2)*numparam);
	std::string parameter_style;
	q= convert_parameters(parameter_style, q);
	PGresult *res2= PQprepare(connection, name, q, numparam, types2);
	if (!res2)
	{
		if (Query::debugging.on) std::cerr << "PQprepare failed: " << name << '\n';
		throw(SQLerror(q,1,"server error"));
	}
	if (PQresultStatus(res2) != PGRES_COMMAND_OK)
	{
		std::string msg= PQresultErrorMessage(res2);
		unsigned stat=PQresultStatus(res2);
		PQclear(res2);
		last_error=stat;
		if (Query::debugging.on) std::cerr << "Error: " << stat << '\t' << msg << '\n';
		throw(SQLerror(q,stat,msg));
	}
	PQclear(res2);
	if (Query::debugging.on)
	{
		fprintf(stderr,"Prepared %s for %s\n", name, q);
	}
	res= new PQ_Prepared_Statement;
	res->conn=this;
	res->name=name;
	res->numparam=numparam;
	res->query= q;
	res->parameters= std::vector< ::Oid>(types2, types2+numparam);
	return res;
}

struct PQ_Cursor : resultsPQ
{
	std::string name;
//	unsigned lines;
//	bool open;

//	virtual unsigned LinesAffected() const throw() { return lines; }
	virtual ManuProC::Query_result_row* Fetch();

	PQ_Cursor() {} //: lines(), open() {}
	~PQ_Cursor()
	{
		std::string q= "CLOSE "+name;
		conn->execute(q.c_str());
	}
};

ManuProC::Query_result_row* PQ_Cursor::Fetch()
{
	parameters.clear();
	query= "FETCH NEXT FROM "+name;
	execute();
	return resultsPQ::Fetch();
}

ManuProC::Query_result_base* connectionPQ::open_cursor(char const* name, char const* q, unsigned num) throw(SQLerror)
{
	std::string parameter_style;
	q= convert_parameters(parameter_style, q);

	PQ_Cursor *res2= new PQ_Cursor;
	res2->conn= this;
	res2->missing_params = num;
	res2->query= std::string("DECLARE ")+name+" CURSOR FOR "+q;
	res2->name= name;
	if (!num) res2->execute();
	last_error=0;
	return res2;
}

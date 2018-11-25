// $Id: FetchIStream_common.cc,v 1.44 2006/08/03 11:18:28 christof Exp $
/*  libcommonc++: ManuProC's main OO library
 *  Copyright (C) 2001-2005 Adolf Petig GmbH & Co. KG
 *  		written by Christof Petig
 *  Copyright (C) 2006-2018 Christof Petig
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

#include <locale.h>
#include <cassert>
#include <ManuProCConfig.h>
#include <Misc/Query.h>
#include <Misc/SQLerror.h>
#include <Misc/itos.h>
#include <Misc/pg_type.h>
#include <cstdlib>
 #include <ecpgerrno.h>
 #include <ecpgerrno.h>

#ifdef MPC_SQLITE
#include <sqlite3.h>
#include <Misc/dbconnect.h>
#if defined(WIN32) && !defined(__MINGW32__)
#	include <string>
#	define STRNCASECMP	_strnicmp
#else
#	include <strings.h>
#	define STRNCASECMP	strncasecmp
#endif

#define ECPG_TOO_MANY_ARGUMENTS         -201
#define ECPG_TOO_FEW_ARGUMENTS          -202
#define ECPG_MISSING_INDICATOR          -213  // in sync with ecpgerrno.h
#define ECPG_DATA_NOT_ARRAY             -215  // in sync with ecpgerrno.h
#define ECPG_UNKNOWN_DESCRIPTOR         -240
#define ECPG_INVALID_DESCRIPTOR_INDEX   -241
#endif

Query::debug_environment::debug_environment() : on(), time_queries()
{  char *env=getenv("SQLOPT");
   if (!env) return;
   std::string senv=env;
   if (senv=="-E") on=true;
   else if (senv=="-T") time_queries=true;
   else if (senv=="-C") count_queries=true;
}

Query::debug_environment::~debug_environment()
{ if (count_queries)
  { for (std::map<std::string,unsigned>::const_iterator i=counts.begin();
      i!=counts.end();++i)
    { std::cerr << i->second << "x " << i->first << '\n';
    }
  }
}

void Query::swap(Query &b)
{
#ifdef MPC_POSTGRESQL
//  std::swap(descriptor,b.descriptor);
#endif
//  std::swap(eof,b.eof);
//  std::swap(line,b.line);
//  std::swap(result,b.result);
#ifdef MPC_SQLITE
//  std::swap(nfields,b.nfields);
#endif
  std::swap(implementation_specific,b.implementation_specific);
  std::swap(query,b.query);
  std::swap(params,b.params);
  std::swap(num_params,b.num_params);
  std::swap(embedded_iterator,b.embedded_iterator);
  std::swap(error,b.error);
  std::swap(lines,b.lines);
  std::swap(prepare,b.prepare);
  std::swap(portal_name,b.portal_name);
}

Query::debug_environment Query::debugging;

void Query_Row::mythrow(const SQLerror &e)
{  if (Query::debugging.on) std::cerr << e << '\n';
   throw e;
}

void Query::Check100() const throw(SQLerror)
{
	if (!backend || !implementation_specific || !implementation_specific->complete())
		 Query_Row::mythrow(SQLerror(query,ECPG_TOO_FEW_ARGUMENTS,"to few input parameter"));
//	if (!params.complete())
   if (!implementation_specific->LinesAffected())
	   Query_Row::mythrow(SQLerror(query,100,"no lines selected"));
}

Query &Query::operator>>(const check100 &s)
{  Check100();
   return *this;
}

#define DEBUG_FIS(x) std::cerr << x << '\n'

Query_Row &Query_Row::operator>>(char &c)
{  std::string s;
   *this >> s;
   if (s.size()>1) mythrow(SQLerror("Query_Row>>char&",ECPG_DATA_NOT_ARRAY,"data too long"));
   c=*s.c_str();
   return *this;
}

Query_Row &Query_Row::operator>>(int &i)
{  std::string s;
   *this >> s;
   i=strtol(s.c_str(),0,10);
   return *this;
}

Query_Row &Query_Row::operator>>(long &i)
{  std::string s;
   *this >> s;
   i=strtol(s.c_str(),0,10);
   return *this;
}

#if !defined(__MINGW32__) && !defined(WIN32)
Query_Row &Query_Row::operator>>(long long &i)
{  std::string s;
   *this >> s;
   i=strtoll(s.c_str(),0,10);
   return *this;
}
#endif

Query_Row &Query_Row::operator>>(unsigned &i)
{  std::string s;
   *this >> s;
   i=strtoul(s.c_str(),0,10);
   return *this;
}

Query_Row &Query_Row::operator>>(unsigned long &i)
{  std::string s;
   *this >> s;
   i=strtoul(s.c_str(),0,10);
   return *this;
}

Query_Row &Query_Row::operator>>(float &f)
{  double d=0;
   *this >> d;
   f=d;
   return *this;
}

Query_Row &Query_Row::operator>>(double &f)
{  std::string s;
   *this >> s;
   /* Make sure we do NOT honor the locale for numeric input */
   /* since the database gives the standard decimal point */
   std::string oldlocale= setlocale(LC_NUMERIC, NULL);
   setlocale(LC_NUMERIC, "C");
   f=strtod(s.c_str(),0);
   setlocale(LC_NUMERIC, oldlocale.c_str());
   return *this;
}

Query_Row &Query_Row::operator>>(bool &b)
{  std::string s;
   *this >> s;
   b=s[0]=='t' || s[0]=='T' || s[0]=='1';
   return *this;
}

// ============================ Query =======================

// copied from ecpg/lib/execute.c
const char *ArgumentList::next_insert(const char *text)
{   const char       *ptr = text;
    bool        string = false;

    for (; *ptr != '\0' && (*ptr != '?' || string); ptr++)
    {   if (*ptr == '\\')       /* escape character */
            ptr++;
        else if (*ptr == '\'')
            string = string ? false : true;
    }

    return (*ptr == '\0') ? 0 : ptr;
}

#if 0
Query_types::Oid ArgumentList::type_of(const_iterator const& which) const
{ return types[which-begin()];
}

bool ArgumentList::is_binary(const_iterator const& which) const
{ return binary[which-begin()];
}

bool ArgumentList::is_null(const_iterator const& which) const
{ return null[which-begin()];
}

std::string const& ArgumentList::get_string(const_iterator const& which) const
{
	return params[which-begin()];
}
#endif

ManuProC::ArgumentEntry::ArgumentEntry(std::string const &s2)
	: type(TEXTOID), null(), s(s2), i(), f()
{}
ManuProC::ArgumentEntry::ArgumentEntry(int64_t a)
	: type(INT8OID), null(), s(), i(a), f()
{}
ManuProC::ArgumentEntry::ArgumentEntry(int32_t a)
	: type(INT4OID), null(), s(), i(a), f()
{}
ManuProC::ArgumentEntry::ArgumentEntry(double f2)
	: type(FLOAT8OID), null(), s(), i(), f(f2)
{}
ManuProC::ArgumentEntry::ArgumentEntry(bool b)
	: type(BOOLOID), null(), s(), i(b), f()
{}
ManuProC::ArgumentEntry::ArgumentEntry(char const* s2)
	: type(TEXTOID), null(), s(s2), i(), f()
{}

static bool needs_quotes(Query_types::Oid type)
{ switch (type)
  { case CHAROID:
    case DATEOID:
    case INTERVALOID:
    case TIMESTAMPTZOID:
    case TEXTOID: return true;

    case INT8OID:
    case INT4OID:
    case NUMERICOID:
    case FLOAT4OID:
    case FLOAT8OID:
    case BOOLOID: return false;

    default: std::cerr << "Oid " << type << " is unknown\n";
      return true;
  }
}

#if 0 // now part of backend
std::string Query::standardize_parameters(std::string const& in)
{
  std::string expanded;
  const char *p=in.c_str();
  const char *last=p;
  unsigned idx=1;

  do
  {  p=ArgumentList::next_insert(p);
     if (!p) expanded+=last;
     else
     {  expanded+=std::string(last,p-last);
        expanded+="$"+itos(idx++);
        ++p;
        last=p;
     }
  } while(p);
  return expanded;
}
#endif

void Query::Execute_if_complete()
{  if (params.complete() && !already_run())
   {
#if 0 //ndef USE_PARAMETERS
      std::string expanded;
      const char *p=query.c_str();
      const char *last=p;
      ArgumentList::const_iterator piter=params.begin();

      do
      {  p=ArgumentList::next_insert(p);
         if (!p) expanded+=last;
         else
         {  expanded+=std::string(last,p-last);
            assert(piter!=params.end());
            Oid type=params.type_of(piter);
            if(params.is_null(piter))
	    {expanded+="null";
	    }
            else
            if(needs_quotes(type))
            { expanded+="'"+*piter+"'";
            }
            else expanded+=*piter;
            ++p;
            ++piter;
            last=p;
         }
      } while(p);
      query=expanded;
#else // replace ? by $N
      //if (!params.empty()) query=standardize_parameters(query);
#endif
      Execute();
   }
}

Query &Query::add_argument(const std::string &s, Oid type)
{  params.add_argument(s,type);
   Execute_if_complete();
   return *this;
}

ArgumentList &ArgumentList::operator<<(Query_types::null_s n)
{ if (complete())
      Query_Row::mythrow(SQLerror(std::string(),ECPG_TOO_MANY_ARGUMENTS,"too many arguments"));
  params.push_back(ManuProC::ArgumentEntry(n));
  --params_needed;
  return *this;
}

ArgumentList &ArgumentList::add_argument(const std::string &x, Oid type)
{  if (complete())
      Query_Row::mythrow(SQLerror(std::string(),ECPG_TOO_MANY_ARGUMENTS,"too many arguments"));
   ManuProC::ArgumentEntry e(x, type);
   params.push_back(e);
  --params_needed;
   return *this;
}

ArgumentList &ArgumentList::add_argument(ManuProC::ArgumentEntry const& e)
{  if (complete())
      Query_Row::mythrow(SQLerror(std::string(),ECPG_TOO_MANY_ARGUMENTS,"too many arguments"));
   params.push_back(e);
  --params_needed;
   return *this;
}

#if 1
static bool transparent_char(unsigned char x)
{ return (true
#if 1 //ndef MPC_SQLITE // escape these
          && ((' '<=x&&x<=126) || (128<=x))
          && x!='\\'
#endif
#if 0 // ndef USE_PARAMETERS
          && x!='\''
#endif
        );
}
#endif

ArgumentList &ArgumentList::operator<<(const std::string &str)
{ // do we need to escape it?
  // do we need bytea?
#if 1 // (defined(MPC_SQLITE) && defined(USE_PARAMETERS)) || (!defined(MPC_SQLITE) && defined(USE_PARAMETERS))
  return add_argument(str,TEXTOID);
#else
  std::string p;
  for (std::string::const_iterator i=str.begin();i!=str.end();++i)
  {  if (transparent_char(*i)) p+=*i;
     else
     { p+='\\';
       p+='0'+((*i>>6)&0x3);
       p+='0'+((*i>>3)&0x7);
       p+='0'+(*i&0x7);
     }
  }
  return add_argument(p,TEXTOID);
#endif
}
template<>
const Query_types::Oid Query::NullIf_s<std::string>::postgres_type=TEXTOID;
template<>
const Query_types::Oid Query::NullIf_s<char const*>::postgres_type=TEXTOID;

ArgumentList &ArgumentList::operator<<(long i)
{
  if (sizeof(i)==sizeof(int32_t))
	return add_argument(ManuProC::ArgumentEntry((int32_t)i));
  else
	return add_argument(ManuProC::ArgumentEntry((int64_t)i));
}
template<> const Query_types::Oid Query::NullIf_s<long>::postgres_type=INT4OID;
template<> const Query_types::Oid Query::NullIf_s<int>::postgres_type=INT4OID;

ArgumentList &ArgumentList::operator<<(unsigned long i)
{
  if (sizeof(i)==sizeof(uint32_t)) // desperate attempt, I hope we don't use 32 bit numbers
	return add_argument(ManuProC::ArgumentEntry((int32_t)i));
  else
	return add_argument(ManuProC::ArgumentEntry((int64_t)i));
}
template<> const Query_types::Oid Query::NullIf_s<unsigned long>::postgres_type=INT4OID;
template<> const Query_types::Oid Query::NullIf_s<unsigned int>::postgres_type=INT4OID;

// the API can't handle unsigned 64bit, only unsigned 63bit ...
ArgumentList &ArgumentList::operator<<(unsigned long long i)
{  return add_argument(ManuProC::ArgumentEntry((int64_t)i));
}
template<> const Query_types::Oid Query::NullIf_s<unsigned long long>::postgres_type=INT8OID;

ArgumentList &ArgumentList::operator<<(double i)
{  return add_argument(ManuProC::ArgumentEntry(i));
}
template<> const Query_types::Oid Query::NullIf_s<double>::postgres_type=FLOAT8OID;
template<> const Query_types::Oid Query::NullIf_s<float>::postgres_type=FLOAT4OID;

ArgumentList &ArgumentList::operator<<(bool i)
{
  return add_argument(ManuProC::ArgumentEntry(i));
}
template<> const Query_types::Oid Query::NullIf_s<bool>::postgres_type=BOOLOID;

ArgumentList &ArgumentList::operator<<(char i)
{  char x[8];
   if (transparent_char(i))
   { x[0]=i;
     x[1]=0;
   }
   else
#if 0 // def MPC_SQLITE // there's no escaping here, hopefully we use parameters
     return add_argument(std::string(1,i),TEXTOID);
#else
   { x[0]='\\';
     x[1]='0'+((i>>6)&0x3);
     x[2]='0'+((i>>3)&0x7);
     x[3]='0'+(i&0x7);
     x[4]=0;
   }
#endif
   return add_argument(x,CHAROID);
}
template<> const Query_types::Oid Query::NullIf_s<char>::postgres_type=CHAROID;

ArgumentList &ArgumentList::operator<<(const ArgumentList &list)
{  for (const_iterator i=list.begin();i!=list.end();++i)
   { add_argument(*i);
   }
   return *this;
}

Query_Row &Query::Fetch()
{  Fetch(embedded_iterator);
   return embedded_iterator;
}

Query &Query::operator>>(Query_Row &s)
{  Fetch(s);
   return *this;
}

Query_Row &Query::FetchOne()
{  ThrowOnBad(__FUNCTION__);
   if (portal_name.empty()) Check100();
   Fetch(embedded_iterator);
   if (backend->Type()!=ManuProC::Connection::C_SQLite) // doesn't work yet
   {
	   Query_Row dummy;
	   Fetch(dummy);
	   if (good()) Query_Row::mythrow(SQLerror(__FUNCTION__,-2,"more than one result"));
   }
   return embedded_iterator;
}

bool Query_Row::good() const
{
	return impl && naechstesFeld<impl->columns();
}

void Query::ThrowOnBad(const char *where) const
{  if (error!=0 && error!=100)
   {  
      Query_Row::mythrow(SQLerror(__FUNCTION__,error,"unspecific bad result"));
   }
}

std::string Query_Row::getFieldName() const
{
	if (!impl)
		return "?";
	return impl->getFieldName(naechstesFeld);
}

// ====================== SQLite =================================

#if 1 //def MPC_SQLITE
Query_Row::Query_Row() //const char *const *res, unsigned _nfields, int line)
	: naechstesFeld()/*, zeile() / *, is_fake(), fake_null(),
	  result(res), nfields(_nfields)*/, impl()
{}

Query_Row &Query_Row::operator>>(std::string &str)
{
	str= impl->text(naechstesFeld);
	++naechstesFeld;
	return *this;
}

void Query_Row::ThrowIfNotEmpty(const char *where)
{}

int Query_Row::getIndicator() const
{
  if(!impl)
  {
//    Query_Row::mythrow(SQLerror(__FUNCTION__,100,"probably no lines selected"));
  }
	return impl->indicator(naechstesFeld);
}

void Query::Execute() throw(SQLerror)
{
	// pass all parameters
	if(!backend)
          Query_Row::mythrow(SQLerror(__FILELINE__,-1,"no valid connection"));

	if (prepare)
	{
		implementation_specific= prepare->prep->execute();
		if(!implementation_specific)
		     Query_Row::mythrow(SQLerror(__FILELINE__,-1,"executing prepared statement failed"));
		for (ArgumentList::const_iterator i=params.begin();i!=params.end();++i)
		{
			implementation_specific->AddParameter(*i);
		}
		error= backend->LastError();
		lines= implementation_specific->LinesAffected();
	}
	else if (params.empty() && portal_name.empty())
	{
		implementation_specific= backend->execute2(query.c_str());
		error= backend->LastError();
		if(!implementation_specific)
                  Query_Row::mythrow(SQLerror(__FILELINE__,-1,"no valid connection"));
		lines= implementation_specific->LinesAffected();
	}
	else
	{
		if (portal_name.empty()) implementation_specific= backend->execute_param(query.c_str(), params.size());
		else implementation_specific= backend->open_cursor(portal_name.c_str(), query.c_str(), params.size());
		if(!implementation_specific)
                  Query_Row::mythrow(SQLerror(__FILELINE__,-1,"no valid connection"));
		for (ArgumentList::const_iterator i=params.begin();i!=params.end();++i)
		{
			implementation_specific->AddParameter(*i);
		}
		error= backend->LastError();
		// a cursor declaration won't return tuples, fix this
		if (!portal_name.empty() && error==100) error=0;
		lines= implementation_specific->LinesAffected();
	}
}

void Query::Fetch(Query_Row &is)
{
	is.impl= implementation_specific->Fetch();
	if (!is.impl) error= 100;
	is.naechstesFeld=0;
}

//------------------------------------------------------------------------------------------

Query::Query() : num_params(), error(), lines(), prepare(), backend(ManuProC::get_database()), implementation_specific()
{ 
  params.setNeededParams(0); 
}

//------------------------------------------------------------------------------------------

Query::Query(const std::string &command)
#if 0 // c++11 +
: Query(ManuProC::get_database(), command)
{}
#else
: query(command), num_params(), prepare(),
	error(ECPG_TOO_FEW_ARGUMENTS), lines(), backend(ManuProC::get_database()), implementation_specific()
{
	// old C++99 style of constructor delegation
	construct_from_string();
}
#endif

void Query::construct_from_string()
{
	char const *p=query.c_str();
	while ((p=ArgumentList::next_insert(p))) { ++num_params; ++p; }
	params.setNeededParams(num_params);
	Execute_if_complete();
}

Query::Query(Handle<ManuProC::Connection_base> const& conn, const std::string &command)
: query(command), num_params(), prepare(),
	error(ECPG_TOO_FEW_ARGUMENTS), lines(), backend(conn), implementation_specific()
{
	construct_from_string();
}

Query::~Query()
{
	if (implementation_specific)
		delete implementation_specific;
}

Query::Query(PreparedQuery &pq)
#if 0 // c++11 +
: Query(ManuProC::get_database(), pq)
{}
#else
: query(pq.command), num_params(pq.no_arguments), prepare(&pq),
	error(ECPG_TOO_FEW_ARGUMENTS), lines(), backend(ManuProC::get_database()), implementation_specific()
{
	// old C++99 style of constructor delegation
    construct_from_prepared();
}
#endif

void Query::construct_from_prepared()
{
	// check for connection across reconnects
	prepare->prep->check_connection(*backend);
	params.setNeededParams(num_params);
	Execute_if_complete();
}

Query::Query(Handle<ManuProC::Connection_base> const& conn, PreparedQuery &pq)
: query(pq.command), num_params(pq.no_arguments), error(ECPG_TOO_FEW_ARGUMENTS),
  	  lines(), backend(conn), prepare(&pq), implementation_specific()
//	  implementation_specific(pq.prep->execute())
{
	construct_from_prepared();
}

// this will work for now, but we really should implement cursors later on:
//   declare portal cursor for ...
//   fetch 1 from portal
//   close portal
Query::Query(std::string const& portal, std::string const& command)
: query(command), num_params(), portal_name(portal), prepare(),
		error(ECPG_TOO_FEW_ARGUMENTS), lines(), backend(ManuProC::get_database()), implementation_specific()
{
	char const *p=query.c_str();
	while ((p=ArgumentList::next_insert(p))) { ++num_params; ++p; }
	params.setNeededParams(num_params);
	Execute_if_complete();
//	Query_Row::mythrow(SQLerror(__FUNCTION__,ECPG_INVALID_DESCRIPTOR_INDEX,"not implemented"));
}

int Query::Code()
{
	return ManuProC::get_database()->LastError();
}

bool Query::already_run() const
{
	return implementation_specific!=0;
}

bool Query::good() const
{
	return !error;
//			backend->LastError();
}

int Query::last_insert_rowid() const
{
	return implementation_specific->last_insert_rowid();
}

void Query::Execute(const std::string &command) throw(SQLerror)
{
   Execute(ManuProC::get_database(), command);
}

void Query::Execute(Handle<ManuProC::Connection_base> const& backend, const std::string &command) throw(SQLerror)
{
   backend->execute(command.c_str());
}

#endif

namespace {
struct FakeDatabaseRow : ManuProC::Query_result_row
{
	Query_Row::Fake val;

   virtual unsigned columns() const throw() { return 1; }
   virtual int indicator(unsigned col) const { return val.is_null ? -1 : 0; }
   virtual char const* text(unsigned col) const { return val.what.c_str(); }
   virtual char const* getFieldName(unsigned col) const { return "fake"; }
	FakeDatabaseRow(Query_Row::Fake const& v) : val(v)
	{}
};
}

Query_Row::Query_Row(Fake const& val)
  : naechstesFeld(), impl(new FakeDatabaseRow(val))
{
}

PreparedQuery::PreparedQuery(std::string const& cmd)
  : prep(), command(cmd), connection(ManuProC::get_database()), no_arguments()
{
//	ArgumentList al;
	unsigned num_params=0;
	char const* p= command.c_str();
	while ((p=ArgumentList::next_insert(p))) { ++num_params; ++p; }
	// TODO: how to specify the type of parameters?
	prep= connection->prepare(0, cmd.c_str(), num_params,0);
	if (prep)
	{
		no_arguments= num_params;
	}
}

PreparedQuery::~PreparedQuery()
{
	if (prep) delete prep;
}

PreparedQuery::PreparedQuery(PreparedQuery const& b)
  : prep(), command(b.command), connection(ManuProC::get_database()), no_arguments(b.no_arguments)
{
}

void PreparedQuery::operator=(PreparedQuery const& b)
{
	if (prep) delete prep;
	prep=0;
	command=b.command;
	connection=ManuProC::get_database();
	no_arguments= b.no_arguments;
}

// no destructor - so copying is easy
void Query_Row::operator=(Query_Row const&b)
{
	impl = b.impl;
	naechstesFeld = b.naechstesFeld;
}

// no destructor - so copying is easy
Query_Row::Query_Row(Query_Row const& b) : naechstesFeld(b.naechstesFeld), impl(b.impl)
{
}

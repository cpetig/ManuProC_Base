// $Id: FetchIStream.pgcc,v 1.38 2006/08/03 11:18:28 christof Exp $
/*  libcommonc++: ManuProC's main OO library
 *  Copyright (C) 2008 Christof Petig
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */


#include <Misc/Query.h>
#include <Misc/dbconnect.h>
#include <iterator>
#include <cassert>
#include <Misc/pg_type.h>
#include <Misc/Transaction.h>

int main()
{  Handle<ManuProC::Connection_base> conn= ManuProC::dbconnect(ManuProC::Connection("","template1"));

   std::cout << "simple use " << Query("select 1").FetchOne<int>() << '\n';

// now test it
  {Query query(conn->Type() == ManuProC::Connection::C_SQLite
		  ? "select '2008-04-24 23:13:04.577404+02','2008-04-23',null,null,200,20.5,'t'"
		  : "select now(),date('yesterday'),null,null,200,20.5,'t'");
   std::string s,s2,s3,s4;
   bool b;
   int i;
   float f;
   int ind3=0;

   query >> s >> s2 >> Query::Row::WithIndicator(s3,ind3)
      >> Query::Row::MapNull(s4,"<NULL>")
      >> i >> f >> b;
   std::cout << s << ", " << s2 << ", " << ind3 << ", " << s4 << '\n';
   std::cout << i << ", " << f << ", " << b << '\n';
  }
  {Query query("select relname from pg_class limit 10");
   std::vector<std::string> v;
   query.FetchArray(v);
   std::copy(v.begin(),v.end(),std::ostream_iterator<std::string>(std::cout,"\n"));
  }
  Query("select cast('test' as varchar(5))").FetchOne<std::string>();
// test nasty error? (overlapping)
{
  std::cout << "------\n";
   Query query2("select 1;");
   std::cout << query2.FetchOne<int>() << '\n';
   Query query3("select 2;");
   std::cout << query3.FetchOne<int>() << '\n';
}
  {int a,b,c,d; std::string x,y,z; bool e,f;
   std::string in="\"'\\";
   Query ("select ?,?,?,'?',?, ?,?, ?,?")
   	<< 1 << Query::null<int>() << "test" << in
   		<< Query::NullIf(1,2) << Query::NullIf(2,2) << true << false
   	>> a >> Query::Row::MapNull(b,-1) >> x >> y >> z
   		>> c >> Query::Row::MapNull(d,-1) >> e >> f;
   std::cout << a << ' ' << b << ' ' << x << ' ' << y << ' ' << z
   		<< ' ' << c << ' ' << d << ' ' << e << ' ' << f << '\n';
  }
   Query ("analyze pg_class");
   // should never select any lines
   Query ("update pg_class set relname=relname where relname='' and oid=0");
   SQLerror::print("should be 100");

//   std::cout << (Query("select ? from pg_class limit 1")
//   	.add_argument("relname",TEXTOID).FetchOne<std::string>()) << '\n';
   std::cout << "this should give a warning:\n";
   Query("select ?");
   SQLerror::print("should be -202");

   try {
   Query ("update pg_class set relname=relname where relname='' and oid=?")
   	<< 0 >> Query::check100();
   std::cerr << "check100 did not work\n";
   return 1;
   } catch (SQLerror &e)
   {  assert(e.Code()==100);
   }

   try {
   Query ("close inexistant");
   std::cerr << "CLOSE does never throw\n";
   } catch (SQLerror &e)
   { std::cerr << "CLOSE does throw\n";
   }

  std::cerr << "--- advanced: portals\n";
  {
   Transaction tr;
   Query query("relname","select relname from pg_class");
   std::vector<std::string> v;
   query.FetchArray(v);
   for (std::vector<std::string>::const_iterator i=v.begin();i!=v.end();++i)
     std::cout << (*i)[0];
   std::cout << '\n';
  }
  std::cerr << "--- advanced: prepare\n";
  {static PreparedQuery pq("select relname from pg_class where relname like ?");
   Query query(pq);
   query << "pg_%";
   std::vector<std::string> v;
   query.FetchArray(v);
   std::cout << v.size() << '\n';
   v.clear();
   Query query2(pq);
   query2 << "sql%";
   query2.FetchArray(v);
   std::cout << v.size() << '\n';
  }
  std::cerr << "--- advanced: declare cursor with params\n";
#ifndef MPC_SQLITE
  { // by foot
   Transaction tr;
   Query q("declare mycurs cursor for select relname from pg_class where relname like ?");
   q << "pg_%";
   Query f("fetch from mycurs");
   std::cout << f.FetchOne<std::string>() << '\n';
   Query("close mycurs");
  }
#endif
  { // by API
   Transaction tr;
   Query q("mycurs","select relname from pg_class where relname like ? limit 1");
   q << "pg_%";
   std::cout << q.FetchOne<std::string>() << '\n';
  }

  {ArgumentList arg;
   arg << 1 << Query::null<int>();
   Query q("select ?,?");
   Query::Row res;
   q << arg >> res;
   std::cout << res.Fetch<std::string>() << ',';
   std::cout << res.FetchMap<std::string>("<NULL>") << '\n';
   res >> Query::Row::check_eol();
  }

   ManuProC::dbdisconnect();
   return 0;
}

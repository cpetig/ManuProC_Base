/* $Id: SQLerror_postgres.h,v 1.9 2006/10/31 16:05:16 christof Exp $ */
/*  libcommonc++: ManuProC's main OO library
 *  Copyright (C) 1998-2000 Adolf Petig GmbH & Co. KG, written by Christof Petig
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

#ifndef SQLERROR_POSTGRES_H
#define SQLERROR_POSTGRES_H

#include <Misc/SQLerror_base.h>
#define __FILELINE__ __FILE__ ":" _sql_STRING__(__LINE__)

#ifdef __cplusplus
class SQLerror_postgres : public SQLerror_base
{  char separator;
   std::string detail;

public:
   explicit SQLerror_postgres(const std::string &context, std::string const& detail=std::string()) throw();
   SQLerror_postgres(const std::string &context,int code,const std::string &name) throw();

//   virtual const char* what() const throw() { return "SQLerror"; }
//   friend std::ostream &operator<<(std::ostream&,const SQLerror_postgres &) throw();

   const SQLerror_postgres &Separator(char sep) { separator=sep; return *this; }
   // member access functions
   
   const std::string Detail() const { return detail; }

//   static void print(const std::string &context,int codeok=0);
//   static void close_cursor(const std::string &c) throw();

//   static void test(const std::string &context,int codeok=0);
////   		throw(SQLerror_postgres);
//   static void test(const std::string &context,const std::string &cursor,
//		int codeok=0); // throw(SQLerror_postgres);
//   static void test(const std::string &context,const char *cursor,
//		int codeok=0) // throw(SQLerror_postgres)
//   {  test(context,std::string(cursor),codeok);
//   }


// with Transaction rollback - please use class Transaction where possible
// I don't like it too much but I have no better idea, CP 2001-6-27

//private:
//   static void test(const std::string &context,bool rollback)
//   { test(context,std::string(),0,rollback); }
//   static void test(const std::string &context,int codeok,bool rollback)
//   { test(context,std::string(),codeok,rollback); }
//   static void test(const std::string &context,const std::string &cursor,
//		int codeok,bool rollback); // throw(SQLerror_postgres);
//   static void rollback_and_throw(const std::string &context)
//   //throw(SQLerror_postgres)
//   {  SQLerror_postgres err(context);
//      rollback();
//      throw(err);
//   }
//   static void rollback() throw();
};

#endif
#endif

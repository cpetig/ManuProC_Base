#pragma once
#ifdef __cplusplus
#include <exception>
#include <string>
#include <Misc/compiler_ports.h>
#include <iostream>
#endif

#define _sql_STRING2__(a) #a
#define _sql_STRING__(a) _sql_STRING2__(a)
#define __FILELINE__ __FILE__ ":" _sql_STRING__(__LINE__)

#ifdef __cplusplus
class Query_Row;

class SQLerror_base : public std::exception
{  std::string context;
   int code;
   std::string name;
   std::string state;
//   char separator;

public:
   virtual ~SQLerror_base() throw() {}
//   SQLerror_sqlite(const std::string &context) throw();
   SQLerror_base(const std::string &context,int code,const std::string &name) throw();

   virtual const char* what() const throw() { return "SQLerror"; }
   virtual void ostream_out(std::ostream&) const throw();
//   friend std::ostream &operator<<(std::ostream&,const SQLerror_base &) throw();

//   const SQLerror_sqlite &Separator(char sep) { separator=sep; return *this; }
   // member access functions
   const std::string Context() const { return context; }
   int Code() const { return code; }
   const std::string Message() const { return name; }
   const std::string State() const { return state; }

   // what is this used for?
   static void print(const std::string &context,int codeok=0);

   static __deprecated void test(const std::string &context,int codeok=0);
//   		throw(SQLerror_base);
   static __deprecated void test(const std::string &context,const std::string &cursor,
		int codeok=0); // throw(SQLerror_base);
   static __deprecated void test(const std::string &context,const char *cursor,
		int codeok=0); // throw(SQLerror_base)

   static __deprecated int SQLCode();

#if __cplusplus>=201103L
   SQLerror_base(SQLerror_base&& b)
   {
	   std::swap(context,b.context);
	   code=b.code;
	   std::swap(name,b.name);
   }
#endif
protected:
   friend class Query_Row;
   // restrict
   SQLerror_base(SQLerror_base const&);

private:
   // forbid
   SQLerror_base const& operator=(SQLerror_base const&);
};

std::ostream &operator<<(std::ostream&,const SQLerror_base &) throw();
#endif

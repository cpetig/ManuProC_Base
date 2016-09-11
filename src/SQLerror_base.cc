
#include <Misc/SQLerror_base.h>

void SQLerror_base::print(std::string const& s, int codeok)
{

}

SQLerror_base::SQLerror_base(SQLerror_base const& b)
: code(b.code), context(b.context), name(b.name)
{
}

SQLerror_base::SQLerror_base(const std::string &c,int c2,const std::string &n) throw()
		: context(c), code(c2), name(n)
{

}

void SQLerror_base::test(std::string const&, int)
{

}

std::ostream& operator<<(std::ostream& o, SQLerror_base const& c) throw ()
{
	o << "SQLerror Code" << c.Code() << " Msg " << c.Message() << " Ctx " << c.Context();
	return o;
}

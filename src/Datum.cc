// $Id: Datum.cc,v 1.40 2006/06/26 07:53:01 christof Exp $
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

/* $Id: Datum.cc,v 1.40 2006/06/26 07:53:01 christof Exp $ */
#include "Datum.h"
#include <time.h>
#include <ctype.h>
#include <Misc/string0.h>
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <Misc/itos.h>
#include <ManuProCConfig.h>
#if defined DEFAULT_DB && defined MANUPROC_WITH_DATABASE // actually we should test for database support
#include <Misc/Query.h>
#endif
#include <i18n.h>
#include <cstring>

const char* const ManuProC::Datum::monate[]={N_("January"),
 		    N_("February"),
 		    N_("March"),
 		    N_("April"),
 		    N_("May"),
 		    N_("June"),
 		    N_("July"),
 		    N_("August"),
 		    N_("September"),
 		    N_("October"),
 		    N_("November"),
 		    N_("December")};

const char* const ManuProC::Datum::month_abbrev[]={N_("Jan"),
 		    N_("Feb"),
 		    N_("Mar"),
 		    N_("Apr"),
 		    N_("May"),
 		    N_("Jun"),
 		    N_("Jul"),
 		    N_("Aug"),
 		    N_("Sep"),
 		    N_("Oct"),
 		    N_("Nov"),
 		    N_("Dec")};

unsigned long ManuProC::Datum::getnum(const unsigned char *s,int len) throw()
{  unsigned long num=0;
   for (;len>0&&*s==' ';len--,s++);
   for (;len>0&&*s;len--,s++) if (isdigit(*s)) num=num*10+*s-'0';
   return num;
}

ManuProC::Datum ManuProC::Datum::today() throw()
{  return ManuProC::Datum(time(0));
}

ManuProC::Datum::Datum(time_t t) throw() :
woche(),woche_jahrdiff(),quart()
{  struct tm *tm=localtime(&t);

   tag=tm->tm_mday;
   monat=tm->tm_mon+1;
   jahr=tm->tm_year+1900;
}

std::string ManuProC::Datum::write_euro() const throw(ManuProC::Datumsfehler)
{	static char ret[16];
	write_euro(ret,sizeof ret);
	return ret;
}

// TODO: This is not internationalized!
const char *ManuProC::Datum::c_str_filled() const throw(ManuProC::Datumsfehler)
{	static char ret[11];
	teste();
	snprintf0(ret,sizeof ret,"%02d.%02d.%04d",tag,monat,jahr);
	return ret;
}

void ManuProC::Datum::write_euro(char *buf,unsigned int size) const throw(ManuProC::Datumsfehler)
{	teste();
	snprintf0(buf,size,"%d.%d.%04d",tag,monat,jahr);
}

const std::string ManuProC::Datum::Short() const throw(Datumsfehler)
{
  teste();
  return itos(Tag())+"."+itos(Monat())+".";
}

std::string ManuProC::Datum::to_iso() const throw(Datumsfehler)
{
 teste();
 return itos(Jahr())+"-"+itos(Monat())+"-"+itos(Tag());
}

std::string ManuProC::Datum::postgres_null_if_invalid() const
{
  try{
   teste();
   return " '"+itos(Jahr())+"-"+itos(Monat())+"-"+itos(Tag())+"' ";
   } catch(Datumsfehler)
   {
    return " null ";
   }
}



void ManuProC::Datum::teste() const throw (ManuProC::Datumsfehler)
{  int falsch=0;
   if (tag<1 || tag>31) falsch|=Datumsfehler::tagfalsch;
   if (monat<1 || monat>12) falsch|=Datumsfehler::monatfalsch;
   if (jahr<1800 || jahr>2999) falsch|=Datumsfehler::jahrfalsch;
   if (falsch)
   {  throw(Datumsfehler(falsch));
   }
}

ManuProC::Datum ManuProC::Datum::Infinity() throw()
{  return ManuProC::Datum(31,12,2999,false);
}

bool ManuProC::Datum::valid() const throw ()
{  if (tag<1 || tag>31) return false;
   if (monat<1 || monat>12) return false;
   if (jahr<1800 || jahr>2999) return false;
   return true;
}

bool ManuProC::Datum::operator<(const Datum &b) const throw(Datumsfehler)
{  teste(); b.teste();
   if (jahr<b.jahr) return true;
   if (jahr>b.jahr) return false;
   if (monat<b.monat) return true;
   if (monat>b.monat) return false;
   return tag<b.tag;
}

int ManuProC::Datum::Julian() const throw(Datumsfehler)
{  teste();
   static const int monatsbeginn[10]=
	{ /* 0,31, */ 59,90,120,151,181,212,243,273,304,334 };
   if (monat<3) return monat<2?tag:tag+31;
   return tag+monatsbeginn[monat-3]+(Schaltjahr(jahr)?1:0);
}

int ManuProC::Datum::Internal() const throw(Datumsfehler)
{  teste();
//const TageProVierJahre=1461; this uses integer arithmetic
   //return ((jahr-1900)*TageProVierJahre)/4+Julian();
   return (int)((jahr-1900)*365.25)+Julian()-2;
   // Julian beginnt bei 1 (statt 0)
   // 1900 ist kein Schaltjahr obwohl so gerechnet, d.h. bis
   // zum 1.3.1900 wird falsch gerechnet ...
}

int ManuProC::Datum::IntRepresentation() const throw(Datumsfehler)
{  teste();
   return jahr*10000+monat*100+tag;
}

int ManuProC::Datum::operator-(const Datum &b) const throw(Datumsfehler)
{  return Internal()-b.Internal();
}

ManuProC::Datum &ManuProC::Datum::operator--()
{  teste();
   if (tag>1) tag--;
   else
   {  if (monat>1) monat--;
      else
      {  jahr--;
         monat=12;
      }
      tag=Tage_in_Monat();
   }
   woche=0;
   return *this;
}

ManuProC::Datum ManuProC::Datum::operator--(int)
{  Datum temp(*this);
   --*this;
   return temp;
}

ManuProC::Datum &ManuProC::Datum::operator++()
{  teste();
   if (tag<Tage_in_Monat()) tag++;
   else
   {  tag=1;
      if (monat<12) monat++;
      else
      {  jahr++;
         monat=1;
      }
   }
   woche=0;
   return *this;
}

ManuProC::Datum ManuProC::Datum::operator++(int)
{  Datum temp(*this);
   ++*this;
   return temp;
}

ManuProC::Datum ManuProC::Datum::operator+(unsigned int tage) const throw(Datumsfehler)
{  teste();
   Datum ret(*this);
   unsigned int ret_tag=ret.tag+tage;
   while (ret_tag>ret.Tage_in_Monat())
   {  ret_tag-=ret.Tage_in_Monat();
      ret.monat++;
      if (ret.monat>12)
      {  ret.monat=1;
         ret.jahr++;
      }
   }
   ret.tag=ret_tag;
   ret.woche=0;
   return ret;
}

ManuProC::Datum ManuProC::Datum::TruncJahr() const throw(Datumsfehler)
{
 teste();
 Datum ret(*this);
 ret.tag=ret.monat=1;
 ret.woche=0;
 return ret;
}

ManuProC::Datum ManuProC::Datum::AddJahr(int jahre) const throw(Datumsfehler)
{
 teste();
 Datum ret(*this);
 if(Schaltjahr(ret.jahr) && ret.tag==29)
   ret.tag=28;
 ret.jahr+=jahre;
 ret.woche=0;
 return ret;
}

ManuProC::Datum ManuProC::Datum::operator-(unsigned int tage) const throw(Datumsfehler)
{  teste();
   Datum ret(*this);
   signed int ret_tag=ret.tag-tage;
   while (ret_tag<1)
   {  ret.monat--;
      if (ret.monat<1)
      {  ret.monat=12;
         ret.jahr--;
      }
      ret_tag+=ret.Tage_in_Monat();
   }
   ret.tag=ret_tag;
   ret.woche=0;
   return ret;
}

#if 1 // defined( __GNUC__) && __GNUC__<4
std::ostream &operator<<(std::ostream&o,const ManuProC::Datum&d) throw()
#else
std::ostream &ManuProC::operator<<(std::ostream&o,const ManuProC::Datum&d) throw()
#endif
{  int w=o.width();
   char f=o.fill();
   o << d.tag << "." << std::setfill(f) << std::setw(w) << d.monat << "." << d.jahr;
   return o;
}

#if 1 // defined( __GNUC__) && __GNUC__<4
std::ostream &operator<<(std::ostream&o,const ManuProC::Datumsfehler &df)
#else
std::ostream &ManuProC::operator<<(std::ostream&o,const ManuProC::Datumsfehler &df)
#endif
{  if (df.falsch&ManuProC::Datumsfehler::tagfalsch) o << "d";
   if (df.falsch&ManuProC::Datumsfehler::monatfalsch) o << "m";
   if (df.falsch&ManuProC::Datumsfehler::jahrfalsch) o << "y";
   return o;
}

const static int seconds_per_day=60*60*24;
const static int seconds_per_week=7*seconds_per_day;

ManuProC::Datum::Datum(const Kalenderwoche &kw) throw(Datumsfehler)
: woche(),woche_jahrdiff(),quart()
{  struct tm tm;
   memset(&tm,0,sizeof tm);
   tm.tm_mday=1;
//   tm.tm_mon=0;
   tm.tm_year=kw.Jahr()-1900;
   tm.tm_hour=12;
   tm.tm_isdst=-1;
   time_t t=mktime(&tm);
   if (!tm.tm_wday) tm.tm_wday=7;
   if (tm.tm_wday<5) // <Friday  1.1. is KW1
      t=t+(5-tm.tm_wday)*seconds_per_day;
   else // >=Friday 1.1. is KW52
      t=t+(12-tm.tm_wday)*seconds_per_day;
   t=t+(kw.Woche()-1)*seconds_per_week;
   *this=Datum(t);
   woche=kw.Woche();
   woche_jahrdiff=jahr-kw.Jahr();
}

//#define DEBUG(x) std::cout << x
#define DEBUG(x)

Kalenderwoche ManuProC::Datum::KW() const throw(Datumsfehler)
{   teste();

   if (jahr<1970 || jahr>=2038) throw Datumsfehler(Datumsfehler::jahrfalsch);
   if(woche) return Kalenderwoche(woche,jahr+woche_jahrdiff);

   struct tm tm;
   bool try_again=true;
   memset(&tm,0,sizeof tm);
   tm.tm_mday=tag;
   tm.tm_mon=monat-1;
   tm.tm_year=jahr-1900;
   tm.tm_hour=12;
   tm.tm_isdst=-1;
   time_t current=mktime(&tm);
   DEBUG(*this << ' ' << current << '\n');

   // calculate Monday, first week 12:00
   memset(&tm,0,sizeof tm);
   tm.tm_year=jahr-1900;
   tm.tm_mday=1;
//   tm.tm_mon=0;
   tm.tm_hour=12;
previous_year:
   DEBUG("1.1."<<(tm.tm_year+1900) << '\n');
   tm.tm_isdst=-1;
   time_t monday=mktime(&tm);
   if (!tm.tm_wday) tm.tm_wday=7;
   if (tm.tm_wday<5) // <Friday  1.1. is KW1
      monday=monday-(tm.tm_wday-1)*seconds_per_day;
   else // >=Friday 1.1. is KW52
      monday=monday+(8-tm.tm_wday)*seconds_per_day;
   DEBUG("monday " << monday << '\n');
   if (current<monday)
   {  if (!try_again)
      {  DEBUG("current<monday !try_again " << tm.tm_year << '\n');
         return Kalenderwoche(53,1900+tm.tm_year-1);
      }
      tm.tm_year--;
      goto previous_year;
   }
   // diese Rundung ist wegen Sommerzeit erforderlich
   int _woche=(current-monday+60*60)/seconds_per_week+1;
   if (_woche>52 && try_again)
   {  tm.tm_year++;
      try_again=false;
      goto previous_year;
   }
   DEBUG(_woche<<'\''<<tm.tm_year+1900 << '\n');
   woche=_woche;
   woche_jahrdiff=jahr-tm.tm_year+1900;
   return Kalenderwoche(woche,tm.tm_year+1900);
}

int ManuProC::Datum::Wochentag(void) const throw(Datumsfehler)
{  teste();
   struct tm tm;
   memset(&tm,0,sizeof tm);
   tm.tm_mday=tag;
   tm.tm_mon=monat-1;
   tm.tm_year=jahr-1900;
   tm.tm_hour=12;
   tm.tm_isdst=-1;
   mktime(&tm);
   if (!tm.tm_wday) tm.tm_wday=7;
   return tm.tm_wday-1;
}


ManuProC::Datum::Datum(int t, int m, int j,bool expandyear) throw(Datumsfehler)
  : woche(),woche_jahrdiff(),quart(),tag(t),monat(m),jahr(j)
{  if (expandyear && jahr<100)
   {  if (jahr<70) jahr+=100;
      jahr+=1900;
   }
   teste();
}

// damit man endlich mal einen Breakpoint drauf setzen kann ...
ManuProC::Datumsfehler::Datumsfehler(int _falsch) throw()
	:  falsch(_falsch)
{}

#if defined DEFAULT_DB && defined MANUPROC_WITH_DATABASE // actually we should test for database support
#include <Misc/pg_type.h>

template<>
const Query_types::Oid Query::NullIf_s<ManuProC::Datum>::postgres_type=DATEOID;

Query::Row &operator>>(Query::Row &is, ManuProC::Datum &v)
{  std::string s;
   int ind;
   is >> Query::Row::WithIndicator(s,ind);
   if (ind==-1) v=ManuProC::Datum();
   else v.from_postgres(s.c_str());
   return is;
}

ArgumentList &operator<<(ArgumentList &q, const ManuProC::Datum &v)
{  if (!v) q << Query::null_s(Query::NullIf_s<ManuProC::Datum>::postgres_type);
   else q.add_argument(itos(v.Jahr())+"-"+itos(v.Monat())+"-"+itos(v.Tag()),
         Query::NullIf_s<ManuProC::Datum>::postgres_type);
   return q;
}

#endif

// there seems to be no real rule on how this is to be stored
ManuProC::Datum ManuProC::Datum::from_access(char const* f) throw(Datumsfehler,Formatfehler)
{
  if (!*f) return Datum();
#if 0
  if (strlen(f)<10) throw Formatfehler();
  if (f[2]!='.' || f[5]!='.') throw Formatfehler();
  return Datum(getnum((const unsigned char*)f,2),getnum((const unsigned char*)f+3,2),getnum((const unsigned char*)f+6,4));
#else
  if (!strncmp(f,"01/01/00",8)) return Datum();
  if (strlen(f)<8) throw Formatfehler();
  if (f[2]=='.' && f[5]=='.' && isdigit(f[6])&& isdigit(f[8])) // german style?
  {
    return Datum(getnum((const unsigned char*)f,2),getnum((const unsigned char*)f+3,2),getnum((const unsigned char*)f+6,4));
  }
  if (f[2]!='/' || f[5]!='/') throw Formatfehler();
  return Datum(getnum((const unsigned char*)f+3,2),getnum((const unsigned char*)f,2),getnum((const unsigned char*)f+6,2));
#endif
}

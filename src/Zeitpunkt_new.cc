// $Id: Zeitpunkt_new.cc,v 1.25 2006/06/26 07:53:01 christof Exp $
/*  libcommonc++: ManuProC's main OO library
 *  Copyright (C) 1998-2005 Adolf Petig GmbH & Co. KG, written by Christof Petig
 *  Copyright (C) 2008-2012 Christof Petig
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

#define CENTRAL_EUROPE

#include <Misc/Zeitpunkt_new.h>
#include <cassert>
#include <ManuProCConfig.h>
#include <Misc/itos.h>
#include <Misc/Ausgabe_neu.h>
#if defined DEFAULT_DB && defined MANUPROC_WITH_DATABASE // actually we should test for database support
#include <Misc/Query.h>
#endif
#include <iostream>
#include <stdlib.h>

Zeitpunkt_new &Zeitpunkt_new::Precision(precision p)
{ prec=p;
  switch (prec)
   {  case days: hour=0; // fall through
      case hours: minute=0;
      case minutes: second=0;
      case seconds: microsecond=0;
      case milliseconds: microsecond-=microsecond%1000;
      case microseconds: break;
      default: assert(0);
   }
  return *this;
}

void Zeitpunkt_new::normalize()
{

   while (microsecond>=1000000)
   {microsecond-=1000000; second++;}
   while (second>=60)
   {second-=60; minute++;}
   while (minute>=60)
   {minute-=60; hour++;}
   while (hour>=24)
   {hour-=24; datum++;}
}


void Zeitpunkt_new::Round(precision p)
{ switch (p)
   {  case days: if (hour>=12) ++datum; break;
      case hours: if (minute>=30) ++hour; break;
      case minutes: if (second>=30) ++minute; break;
      case seconds: if (microsecond>=500000) ++second; break;
      case milliseconds: if ((microsecond%1000)>=500) microsecond+=500; break;
      case microseconds: break;
      default: assert(0);
   }
  normalize();
  Precision(p);
}

long Zeitpunkt_new::diff(const Zeitpunkt_new &b, precision destprec) const throw()
{  switch (destprec)
   {  case days: return datum-b.datum;
      case hours: return (datum-b.datum)*24+hour-b.hour+(b.minutes_from_gmt-minutes_from_gmt)/60;
      case minutes: return ((datum-b.datum)*24+hour-b.hour)*60+minute-b.minute+(b.minutes_from_gmt-minutes_from_gmt);
      case seconds:
         return (((datum-b.datum)*24+hour-b.hour)*60+minute-b.minute+(b.minutes_from_gmt-minutes_from_gmt))*60+second-b.second;
      case milliseconds:
         return ((((datum-b.datum)*24+hour-b.hour)*60+minute-b.minute+(b.minutes_from_gmt-minutes_from_gmt))*60+second-b.second)*1000+(microsecond-b.microsecond)/1000;
      case microseconds:
         return ((((datum-b.datum)*24+hour-b.hour)*60+minute-b.minute+(b.minutes_from_gmt-minutes_from_gmt))*60+second-b.second)*1000000+microsecond-b.microsecond;
      default: assert(0); abort();
   }
}

int Zeitpunkt_new::operator-(const Zeitpunkt_new &b) const throw()
{  return diff(b,prec<b.prec?prec:b.prec);
}

// not tested with different time zones
bool Zeitpunkt_new::operator<(const Zeitpunkt_new &b) const throw()
{  int prec2=prec<b.prec?prec:b.prec;
   int mdiff=b.minutes_from_gmt-minutes_from_gmt;
   int hdiff=mdiff/60;
   mdiff%=60;
   if (datum<b.datum) return true;
   if (datum!=b.datum || prec2==days) return false;
   if (hour<(b.hour-hdiff)) return true;
   if (hour>(b.hour-hdiff) || prec2==hours) return false;
   if (minute<(b.minute-mdiff)) return true;
   if (minute>(b.minute-mdiff) || prec2==minutes) return false;
   if (second<b.second) return true;
   if (second>b.second || prec2==seconds) return false;
   return microsecond<b.microsecond;
}

const ManuProC::Datum &Zeitpunkt_new::Datum() const throw()
{  return datum;
}

Zeitpunkt_new Zeitpunkt_new::operator+(const Zeitpunkt_new::Minutes &dist) const throw()
{  Zeitpunkt_new ret(*this);
   ret.minute+=int(dist);
   ret.hour+=ret.minute/60;
   ret.minute%=60;
   ret.datum+=ret.hour/24;
   ret.hour%=24;
   normalize_TZ();
   return ret;
}

// this is not the same as +Hours(24)
Zeitpunkt_new Zeitpunkt_new::operator+(const Zeitpunkt_new::Days &dist) const throw()
{  Zeitpunkt_new ret(*this);
   ret.datum+=int(dist);
   normalize_TZ();
   return ret;
}

// false if different time zones cross day/hour boundaries
bool Zeitpunkt_new::operator==(const Zeitpunkt_new &b) const throw()
{  int prec2=prec<b.prec?prec:b.prec;
   int mdiff=b.minutes_from_gmt-minutes_from_gmt;
   int hdiff=mdiff/60;
   mdiff%=60;
   if (datum!=b.datum) return false;
   if (prec2==days) return true;
   if (hour!=b.hour-hdiff) return false;
   if (prec2==hours) return true;
   if (minute!=b.minute-mdiff) return false;
   if (prec2==minutes) return true;
   if (second!=b.second) return false;
   if (prec2==seconds) return true;
   return microsecond==b.microsecond;
}

#ifdef WIN32 // complicated but might work
time_t timegm(struct tm * tm)
{
  time_t res = mktime(tm);
  struct tm * wrong_dir= gmtime(&res);
  time_t wrong_dir_s = mktime(wrong_dir);
  return res + (res - wrong_dir_s);
}
#endif

Zeitpunkt_new::operator time_t() throw()
{  struct tm tm;
   tm.tm_sec=prec>=seconds?second:0;
   tm.tm_min=prec>=minutes?minute:0;
   tm.tm_hour=prec>=hours?hour:0;
   tm.tm_mday=datum.Tag();
   tm.tm_mon=datum.Monat()-1;
   tm.tm_year=datum.Jahr()-1900;
   tm.tm_isdst=0;
#if 0 // def WIN32
   return _mkgmtime(&tm)-minutes_from_gmt*60;
#else
   return timegm(&tm)-minutes_from_gmt*60;
#endif
}

void Zeitpunkt_new::calculate_TZ(int isdst) const throw()
{  struct tm tm;
   tm.tm_sec=prec>=seconds?second:0;
   tm.tm_min=prec>=minutes?minute:0;
   tm.tm_hour=prec>=hours?hour:0;
   tm.tm_mday=datum.Tag();
   tm.tm_mon=datum.Monat()-1;
   tm.tm_year=datum.Jahr()-1900;
   tm.tm_isdst=isdst;
#if defined (__MINGW32__) || defined (_MSC_VER) || defined(WIN32)
   time_t t= mktime(&tm);
   struct tm * wrong_dir= gmtime(&t);
   time_t wrong_dir_s = mktime(wrong_dir);
   minutes_from_gmt=(t - wrong_dir_s)/60;
#else
   mktime(&tm);
   minutes_from_gmt=tm.tm_gmtoff/60;
#endif
}

void Zeitpunkt_new::normalize_TZ() const throw()
{
#ifdef WIN32
   calculate_TZ((minutes_from_gmt*60)!=_timezone);
#else
   calculate_TZ((minutes_from_gmt*60)!=timezone);
#endif
}

Zeitpunkt_new::Zeitpunkt_new(time_t t) throw()
	: microsecond(0), prec(seconds)
{  struct tm *tm(localtime(&t));
   if (tm)
   {
    datum=ManuProC::Datum(tm->tm_mday,tm->tm_mon+1,tm->tm_year+1900);
    hour=tm->tm_hour;
    minute=tm->tm_min;
    second=tm->tm_sec;
#if defined __MINGW32__ || defined _MSC_VER
    struct tm * wrong_dir= gmtime(&t);
    time_t wrong_dir_s = mktime(wrong_dir);
    minutes_from_gmt=(t - wrong_dir_s)/60;
#else
    minutes_from_gmt=tm->tm_gmtoff/60;
#endif
   }
   else
   {
     datum=ManuProC::Datum();
     hour=minute=second=minutes_from_gmt=0;
   }
}

std::string Zeitpunkt_new::write() const
{  if (!valid()) return std::string();
   char buf[64];
   write(PostgresTimestamp(buf,sizeof buf));
   return buf;
}

std::string Zeitpunkt_new::c_str() const
{  if (!valid()) return std::string();
   std::string res= Datum().c_str();
   res+= " "+itos(hour)+":"+(minute<10?"0":std::string())+itos(minute);
   if (second || microsecond) res+=std::string(":")+(second<10?"0":std::string())+itos(second);
   if (microsecond) res+=","+Formatiere((unsigned long)microsecond,0,6,"","",'0');
   return res;
}

std::string Zeitpunkt_new::Short(const ManuProC::Datum &d) const
{ std::string res;
  if (datum!=d) res+=datum.Short()+" ";
  res+=itos(hour)+":"+(minute<10?"0":std::string())+itos(minute);
  if (second || microsecond) res+=std::string(":")+(second<10?"0":std::string())+itos(second);
  if (microsecond) res+=","+Formatiere((unsigned long)microsecond,0,6,"","",'0');
  return res;
}

std::string Zeitpunkt_new::ISO() const
{
 std::string res=datum.ISO();
 res+="T";
 res+=itos0pad(hour,2)+":";
 res+=itos0pad(minute,2)+":"; 
 res+=itos0pad(second,2); 
 res+=minutes_from_gmt>0?"+":"-";
 // make abs
 int m_gmt=minutes_from_gmt>0 ? minutes_from_gmt : -minutes_from_gmt;

 res+=itos0pad(m_gmt/60,2)+":";
 res+=itos0pad(m_gmt%60,2); 

 return res;  
}

std::string Zeitpunkt_new::ISO_Local() const
{
 std::string res=datum.ISO();
 res+=" ";
 res+=itos0pad(hour,2)+":";
 res+=itos0pad(minute,2)+":"; 
 res+=itos0pad(second,2); 
 return res;  
}


#if defined DEFAULT_DB && defined MANUPROC_WITH_DATABASE // actually we should test for database support
#include <Misc/pg_type.h>

Query::Row &operator>>(Query::Row &is, Zeitpunkt_new &v)
{  std::string s;
   int ind;
   is >> Query::Row::WithIndicator(s,ind);
   if (ind==-1) v=Zeitpunkt_new();
   else v=Zeitpunkt_new(s.c_str());
   return is;
}

template<>
const Query_types::Oid Query::NullIf_s<Zeitpunkt_new>::postgres_type=TIMESTAMPTZOID;

ArgumentList &operator<<(ArgumentList &q, const Zeitpunkt_new &v)
{  if (!v.valid()) return q << Query::null_s(Query::NullIf_s<Zeitpunkt_new>::postgres_type);
   q.add_argument(v.write(),Query::NullIf_s<Zeitpunkt_new>::postgres_type);
   return q;
}

#endif

/* $Id: Datum.h,v 1.35 2005/12/14 07:34:57 christof Exp $ */
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

#ifndef DATUM_H
#define DATUM_H

#if defined(WIN32) && !defined(__MINGW32__)
#	pragma warning( disable : 4290 )
#endif

#include <time.h>
#include <exception>
#include <string>
#include <iosfwd>
#include <Misc/Kalenderwoche.h>
#include <Misc/compiler_ports.h>

namespace ManuProC
{
class Datumsfehler;
class Datum;
};

//namespace std { class ostream; }

std::ostream &operator<<(std::ostream&,const ManuProC::Datumsfehler&);

/// Ein Datum ist ungültig
class ManuProC::Datumsfehler : public std::exception
{  /// eine Kombination aus tagfalsch, monatfalsch, jahrfalsch
   int falsch;
   const std::string context;
public:
   /** Werte für falsch: */
   static const int tagfalsch=1;
   static const int monatfalsch=2;
   static const int jahrfalsch=4;
   Datumsfehler(int _falsch, const std::string c=std::string()) throw();
   virtual ~Datumsfehler() throw() {}
   friend std::ostream &NOTGCC295(::)operator<<(std::ostream&,const Datumsfehler &);
   virtual const char* what() const throw() { return (std::string("ManuProC::Datumsfehler ")+context).c_str(); }
   const std::string Context() const { return context; }
};

std::ostream &operator<<(std::ostream&,const ManuProC::Datum&) throw();


class ManuProC::Datum
{	mutable unsigned int woche:6;	/* KW */
	mutable int woche_jahrdiff:2;   /* Diff. des Jahres in KW zum Jahr im Datum */
	mutable unsigned int quart:3;	/* Quartal */
	unsigned int tag:5;  	/* tt */
	unsigned int monat:4;	/* mm */
	unsigned int jahr:12;     /* jjjj */


 static const char* const monate[];
 static const char* const month_abbrev[];

public:
	 struct Months // for adding months
	 {
	   unsigned value;
	   explicit Months(unsigned x) : value(x) {}
	 };
	class Formatfehler : public std::exception
	{public:
		virtual const char* what() const throw() { return "ManuProC::Datum::Formatfehler"; }
	};

	/// aus Tag, Monat, Jahr erzeugen (expandyear lässt 2stelliges Jahr zu)
	Datum(int t, int m, int j,bool expandyear=true);

	Datum() throw() : woche(),woche_jahrdiff(),quart(),
	tag(),monat(),jahr()
	{} /* initialize as invalid */

	/** Datum aus const char * erzeugen, Format erraten */
	Datum(const char *datum) :
		woche(),woche_jahrdiff(),quart()
	{  this->from_auto(datum);  }
	/// Datum aus time_t (time(3)) erzeugen
	explicit Datum(time_t t) throw();

	Datum(const Kalenderwoche &kw);

	/// heutiges Datum
	static Datum today() throw();
	static Datum Infinity() throw();

	/// in Menschenlesbare Form bringen (NOT THREAD SAFE!)
	const char *c_str() const;
	const char *c_str_filled() const;

        const std::string Short() const;
	/// in Postgres Repräsentation wandeln
	void write_postgres(char *b,unsigned int sz) const;
   std::string to_iso() const;
   std::string ISO() const;
   std::string postgres_null_if_invalid() const;
	/// in Europäisches Format wandeln
	void write_euro(char *b,unsigned int sz) const;
	void write_i18n(char *b,unsigned int sz) const;
	std::string write_euro() const;
	// for later i18n
	std::string to_locale() const
	{ return write_euro(); }
	/// Gültigkeit testen
	void teste() const;
	/// Datumsformat automatisch raten
        void from_auto(const char *datum,const char **endptr=0);
	/// Datum aus Postgres DATE oder TIMESTAMP erzeugen
        void from_postgres(const char *postgres_timestamp)
        {  from_auto(postgres_timestamp); }
        static Datum from_access(char const* f);
        /// Datum aus Menschenlesbarer Form erzeugen
        void from_c_str(const char *s)
        {  from_auto(s); }

        /// zwei Daten sind gleich?
        bool operator==(const Datum &b) const throw()
        {  return b.tag==tag && b.monat==monat && b.jahr==jahr; }
        bool operator!=(const Datum &b) const throw()
        {  return !(*this==b);  }
        /// erstes Datum vor dem zweiten?
        bool operator<(const Datum &b) const;
        bool operator!() const { return !valid(); }
        /** morgen
            noch nicht implementiert */
        Datum &operator++();
        Datum operator++(int);
        Datum operator+(unsigned int) const;
        Datum operator+(Months const& m) const;
        Datum operator-(unsigned int) const;
        // Schaltet auf das gleiche Datum aber in anderem Jahr
        Datum AddJahr(int) const;
        // gibt als Datum 1. Januar dieses Jahres
        Datum TruncJahr() const;
        Datum &operator+=(unsigned int tage)
        {  return *this=*this+tage;
        }
        /** gestern
            noch nicht implementiert */
        Datum &operator--();
        Datum operator--(int);

        /// Differenz in Tagen
        int operator-(const Datum &b) const;

	/** führende Leerzeichen berlesen, dann Dezimalziffern in ulong wandeln
	    bis zu einer bestimmten Länge */
	static unsigned long getnum(const unsigned char *s,int len) throw();

	unsigned int Tage_in_Monat() const throw()
	{  return Tage_in_Monat(monat,jahr); }
        /// Wieviel Tage hat der Monat
	static unsigned int Tage_in_Monat(int monat,int jahr) throw()
	{  if (monat==2) return Schaltjahr(jahr)?29:28;
	   return (unsigned int) 31-((monat+1+monat/8)&1);
	}
	/// Ist dies ein Schaltjahr (1900<jahr<2099)
	static bool Schaltjahr(int jahr) throw()
	{  return (jahr%4==0); }
	/** Wieviele Tage hat das Jahr,
	    gültig 1901-2099 */
	static int Tage_im_Jahr(int jahr) throw()
	{  return Schaltjahr(jahr)?366:365; }

	/// Der [1..366]. Tag im Jahr
	int Julian() const;
	/** Tage seit 1.1.1900
            <br>(1.1.1 waere möglich gewesen,
	    aber: Kalenderreformationen sind ein Kreuz) */
	int Internal() const;
	/** Integer representation YYYYMMDD */
	int IntRepresentation() const;
	/// Kalenderwoche
	Kalenderwoche KW() const;
	/// Wochentag: 0=Montag 6=Sonntag
	int Wochentag() const;

	/**member access */
	int Tag() const { return tag; }
	int Monat() const { return monat; }
	int Jahr() const { return jahr; }
	int MonatTag() const { return monat*100+tag; } // to compare dates
	                                        // in different years
	int Quartal() const { if(quart) return quart;
				quart=((monat-1)/3)+1;
			      return quart;
			 }
	std::string MonatName() const { return monate[monat-1];}
	static std::string MonatName(int m) { return monate[m-1];}

	friend std::ostream &NOTGCC295(::)operator<<(std::ostream&,const Datum&) throw();

	bool valid() const throw();

};

class Query_Row;
Query_Row &operator>>(Query_Row &is, ManuProC::Datum &v);
class ArgumentList;
ArgumentList &operator<<(ArgumentList &, const ManuProC::Datum &v);

namespace Petig
{  using ManuProC::Datumsfehler;
   using ManuProC::Datum;
}

#endif

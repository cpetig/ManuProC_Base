// $Id: itos.cc,v 1.13 2004/02/12 11:26:43 jacek Exp $
/*  libcommonc++: ManuProC's main OO library
 *  Copyright (C) 2001 Adolf Petig GmbH & Co. KG, written by Jacek Jakubowski
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

#include <cstdio>
#include <Misc/itos.h>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <locale.h>

#ifdef __MINGW32__
//extern "C" { int snprintf(char *str, size_t size, const  char  *format, ...); }
#endif

#ifdef _MSC_VER
#define strcasecmp _stricmp
#define snprintf _snprintf
#endif

std::string itos0p(int i, int c,bool show_plus)
{
  if(i==c) return std::string();
  if(!show_plus || i<0) return itos(i) ;
  return  "+"+itos(i) ;
}

std::string itos0pad(int i,int p)
{
 char buffer[20];
 if(p>20) p=20;
 snprintf(buffer,sizeof buffer,"%0*i",p,i);
 return(buffer);
}


std::string itos(int i)
{
 char buffer[100];
 snprintf(buffer,sizeof buffer,"%i",i);
 return(buffer);
}

std::string utos(unsigned int i)
{
 char buffer[100];
 snprintf(buffer,sizeof buffer,"%u",i);
 return(buffer);
}

std::string dtos(double i)
{
 char *oldlocale;
 oldlocale = strdup(setlocale(LC_NUMERIC, NULL));
 setlocale(LC_NUMERIC, "C");

 std::ostringstream o;
 o << i << char(0);
 o.flush();

 std::string res(o.str());

 setlocale(LC_NUMERIC, oldlocale);
 free(oldlocale);

 return(res);
}

std::string ulltos(unsigned long long i)
{
 char buffer[100];
 snprintf(buffer,sizeof buffer,"%llu",i);
 return(buffer);
}

std::string btos(bool b)
{
 if(b) return "true";
 else  return "false";
}

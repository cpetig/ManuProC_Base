/* $Id: EntryValueIntString.h,v 1.13 2003/04/23 09:19:20 christof Exp $ */
/*  libcommonc++: ManuProC's main OO library
 *  Copyright (C) 2009-2010 Christof Petig
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

#ifndef ENTRYVALUEDOUBLE_H
#define ENTRYVALUEDOUBLE_H

#include <Misc/EntryValueBase.h>
#include <Misc/compiler_ports.h>

class EntryValueDouble : public EntryValueBase
{
protected:
 double val;
 std::string strval;
 int intval;

public:
 EntryValueDouble(double);
   
 virtual int getIntVal() const { return intval;}
 virtual const std::string getStrVal() const { return strval;}
 virtual double getDoubleVal() const { return val; }

 virtual bool operator==(const EntryValueBase &v) const;
 virtual bool operator<(const EntryValueBase &v) const;
 virtual operator double() const;
};

#endif

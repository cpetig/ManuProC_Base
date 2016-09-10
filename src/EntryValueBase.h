/* $Id: EntryValueBase.h,v 1.15 2004/01/20 08:04:44 christof Exp $ */
/*  libcommonc++: ManuProC's main OO library
 *  Copyright (C) 1998-2001 Adolf Petig GmbH & Co. KG, written by Christof Petig
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

#ifndef ENTRY_VALUEBASE_H
#define ENTRY_VALUEBASE_H
#include <Misc/Handles.h>
#include <string>
#include <Misc/compiler_ports.h>
#include <Misc/libManuProC_Base_dll.h>

class EntryValueBase : public HandleContent
// opaque Data Type which is accessible as an int value or as a string
{
public:
 // this requires c++11 but I no longer see this as a restriction
 // especially seeing the utterly complex way with c++98 on MinGW
 static constexpr int int_NaN=-1;
 static constexpr double double_NaN=-1.0;
 virtual operator const std::string() const { return getStrVal(); }
 virtual const std::string getStrVal() const { return std::string(); }
 virtual bool operator==(const EntryValueBase &v) const;
 virtual bool operator<(const EntryValueBase &v) const;
 ~EntryValueBase() {}

// CP: I don't like these functions here, they should get replaced by the 
// correct dynamic_cast construction, we can't add a function here for every 
// type we define
// a string and operator< are needed for proper display, these are not needed

 __deprecated virtual double getDoubleVal() const { return double_NaN; }
 __deprecated virtual bool getBoolVal() const { return false; }
 __deprecated virtual operator double() const;
 __deprecated virtual operator bool() const;

// CP: I'm not happy with these either 
// but they offered nice sorting fallback in the past
 /*__deprecated*/ virtual operator int() const { return getIntVal(); }
 /*__deprecated*/ virtual int getIntVal() const { return int_NaN; }
};

class cH_EntryValue : public Handle<const EntryValueBase>
{
 cH_EntryValue(int forbidden); // do not use this, use a derived class
public:
 cH_EntryValue() // why not a NULL pointer?
 : Handle<const EntryValueBase>(new EntryValueBase())
 {}
 cH_EntryValue(const EntryValueBase *r) 
 : Handle<const EntryValueBase>(r){}
 ~cH_EntryValue() {}
};

#endif // ENTRYVALH

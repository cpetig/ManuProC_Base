/* $Id: IOerror.h,v 1.4 2002/05/09 12:46:00 christof Exp $ */
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

#ifndef IOERROR_H
#define IOERROR_H
#include <string>
#include <exception>

class IOerror : public std::exception
{	int IOerrno;
	std::string name;
   public:
   	virtual const char* what() const throw() { return "IOerror"; }
   	IOerror() throw();
   	IOerror(const std::string &) throw();
   	IOerror(const std::string &,int) throw();
   	std::string toString() const throw();
   	virtual ~IOerror() throw() {}
};

#endif

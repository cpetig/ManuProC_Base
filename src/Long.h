/* $Id: Long.h,v 1.5 2005/10/11 15:16:14 christof Exp $ */
/*  libcommonc++: ManuProC's main OO library
 *  Copyright (C) 2001 Adolf Petig GmbH & Co. KG, written by Christof Petig
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

#ifndef LONG_H
#define LONG_H

#include <Misc/auto_init.h>

// it looks like this class should represent an automatically initializing
// integer value
typedef auto_init<long> Long;
#endif

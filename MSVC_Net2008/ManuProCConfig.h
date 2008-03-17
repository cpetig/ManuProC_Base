/* src/ManuProCConfig.h.  Generated from ManuProCConfig.h.in by configure.  */
/* $Id: ManuProCConfig.h.in,v 1.25 2006/08/03 11:18:28 christof Exp $ */
/*  ManuProCBase: ManuProC's main OO library
 *  Copyright (C) 2004 Adolf Petig GmbH & Co. KG, written by Christof Petig
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef D3FB940D_2AF1_44E4_9631_5ED524D1CEFB
#define D3FB940D_2AF1_44E4_9631_5ED524D1CEFB

/*
   Database and host to use
   */

#define DEFAULT_DB "christof"
#define DEFAULT_DBHOST "localhost"

/* we have support for pgsql/ecpg */
#define MANUPROC_WITH_DATABASE 1

/* which SigC++ version to use (e.g. 0x120) */
#define MPC_SIGC_VERSION 0x200

/* Define to 1 if you want to use the SQLite backend */
/* #undef MPC_SQLITE */

/* 
   Define to 1 if you use ecpg 2.9.
   */
/* #undef OLD_ECPG */

/* deprecated: we use SigC++ 1.2 */
/* #undef SIGC1_2 */

/* Define to 1 if PQprepare is available in libpq. */
#define HAVE_PQPREPARE 1

#endif

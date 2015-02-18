// $Id: SimpleTreeStore.h,v 1.77 2006/08/09 11:12:16 christof Exp $
/*  libKomponenten: GUI components for ManuProC's libcommon++
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

#ifdef __cplusplus
extern "C"
#endif
 int getuid_ManuProC();

#ifdef __cplusplus
#include <string>

void setuid_ManuProC(int id);
namespace ManuProC
{
  std::string GetUserName();
  std::string GetRealName();
  bool IsAdministrator();
}
#endif

#define getuid() getuid_ManuProC()

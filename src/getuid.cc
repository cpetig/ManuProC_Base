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

#include "./getuid.h"
#undef getuid
//#include <WinFileReq.hh>

static int uid_override=-1;

#ifdef WIN32
# include "md5string.h"

# define WINVER 0x0500
# define SECURITY_WIN32
# include <windows.h>
# include <security.h>
# include <iphlpapi.h>
# undef GetUserName

std::string ManuProC::GetUserName()
{
  wchar_t buf[10240];
  DWORD sz=sizeof(buf)/sizeof(*buf);

  if (!GetUserNameExW(NameSamCompatible,buf,&sz))
  { if (!GetUserNameW(buf,&sz)) return 0;
  }
  return WinFileReq::un_wstring(std::wstring(buf,buf+sz));
}

std::string ManuProC::GetRealName()
{
  wchar_t buf[10240];
  DWORD sz=sizeof(buf)/sizeof(*buf);

  if (!GetUserNameExW(NameDisplay,buf,&sz))
    return std::string();
  while (sz>0 && !buf[sz-1]) --sz; // XP counts zeros as part of string ...
  return WinFileReq::un_wstring(std::wstring(buf,buf+sz));
}

bool ManuProC::IsAdministrator()
{
  BOOL b;
  SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
  PSID AdministratorsGroup;
  b = AllocateAndInitializeSid(
      &NtAuthority,
      2,
      SECURITY_BUILTIN_DOMAIN_RID,
      DOMAIN_ALIAS_RID_ADMINS,
      0, 0, 0, 0, 0, 0,
      &AdministratorsGroup);
  if(b)
  {
      if (!CheckTokenMembership( NULL, AdministratorsGroup, &b))
      {
           b = FALSE;
      }
      FreeSid(AdministratorsGroup);
  }
  return(b);
}

// on Windows return the first 4 bytes of the md5 sum of "domain\user"
int getuid_ManuProC()
{
  if (uid_override!=-1) return uid_override;
  std::string md5sum= md5(ManuProC::GetUserName());
  uid_override= *(int*)md5sum.data();
  return uid_override;
}

#else // linux
# include <unistd.h>
# include <pwd.h>
# include <stdlib.h>
# include <stdio.h>
# include <getuid.h>
# undef getuid

int getuid_ManuProC()
{
  if (uid_override!=-1) return uid_override;
  uid_override= getuid();
  return uid_override;
}

std::string ManuProC::GetUserName()
{
  size_t bufsize= sysconf(_SC_GETPW_R_SIZE_MAX);
  if (bufsize==-1) bufsize= 16384;
  char *buf= (char*)malloc(bufsize);
  if (!buf) return "malloc failed";
  struct passwd pwd;
  struct passwd *result= 0;
  int s= getpwuid_r(getuid(), &pwd, buf, bufsize, &result);
  std::string sresult;
  if (!result) sresult="not found";
  else sresult= pwd.pw_name;
  free(buf);
  return sresult;
}

std::string ManuProC::GetRealName()
{
  size_t bufsize= sysconf(_SC_GETPW_R_SIZE_MAX);
  if (bufsize==-1) bufsize= 16384;
  char *buf= (char*)malloc(bufsize);
  if (!buf) return "malloc failed";
  struct passwd pwd;
  struct passwd *result= 0;
  int s= getpwuid_r(getuid(), &pwd, buf, bufsize, &result);
  std::string sresult;
  if (!result) sresult="not found";
  else sresult= pwd.pw_gecos;
  free(buf);
  while (!sresult.empty() && sresult[sresult.size()-1]==',') sresult=sresult.substr(0,sresult.size()-1);
  return sresult;
}

bool ManuProC::IsAdministrator()
{
  return !getuid();
}
#endif

void setuid_ManuProC(int id)
{
  uid_override= id;
}


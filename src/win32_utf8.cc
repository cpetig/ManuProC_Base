#include "win32_utf8.h"

#ifdef WIN32
# include <assert.h>
# include <windows.h>
# include <stdio.h>

std::wstring ManuProC::make_wstring(std::string const& x)
{
  if (x.empty()) return std::wstring();
  wchar_t wstring[10240];
  int res= MultiByteToWideChar(CP_UTF8, 0, x.data(), x.size(), wstring, sizeof(wstring)/sizeof(wchar_t));
  assert(res!=0);
//  size_t r= mbstowcs(wstring,x.c_str(),sizeof(wstring)/sizeof(wchar_t));
//  if (r==(size_t)(-1)) return std::wstring();
  return std::wstring(wstring,wstring+res);
}

std::string ManuProC::un_wstring(std::wstring const& x)
{
  if (x.empty()) return std::string();
  char nstring[10240];
  int res= WideCharToMultiByte(CP_UTF8, 0, x.data(), x.size(), nstring, sizeof(nstring), NULL, NULL);
  assert(res!=0);
//  size_t r= wcstombs(nstring,x.c_str(),sizeof(nstring)/sizeof(char));
//  if (r==(size_t)(-1)) return std::string();
  return std::string(nstring,nstring+res);
}

#undef access
#undef fopen

int access_utf8(const char* name, int md)
{
  return _waccess(ManuProC::make_wstring(name).c_str(), md);
}

FILE *fopen_utf8(const char* name, const char* md)
{
  return _wfopen(ManuProC::make_wstring(name).c_str(), ManuProC::make_wstring(md).c_str());
}

#endif

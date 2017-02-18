// $Id: exception.cc,v 1.12 2003/07/30 09:17:20 christof Exp $
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

// $Id: exception.cc,v 1.12 2003/07/30 09:17:20 christof Exp $
// long explantion at the end

#include <iostream>
#include <Misc/exception.h>
#ifndef _MSC_VER
#include <unistd.h>
#endif
#include <stdlib.h>
#include <typeinfo>
//#include <stddef.h>

#if __GNUC__ == 2 
// taken from gcc/eh_common.h

typedef void * (*__eh_matcher) (void *, void *, void *);

typedef struct __eh_info
{
  __eh_matcher match_function;
  short language;
  short version;
} __eh_info;

// taken from gcc/cp/exception.cc


/* C++-specific state about the current exception.
   This must match init_exception_processing().

   Note that handlers and caught are not redundant; when rethrown, an
   std::exception can have multiple active handlers and still be considered
   uncaught.  */

struct cp_eh_info
{
  __eh_info eh_info;
  void *value;
  void *type;
  void (*cleanup)(void *, int);
  bool caught;
  cp_eh_info *next;
  long handlers;
  void *original_value;
};

/* Language-specific EH info pointer, defined in libgcc2. */

extern "C" cp_eh_info **__get_eh_info ();       // actually void **
#endif

static void print_exception_u()
{  std::cerr << "unexpected std::exception: ";
#if __GNUC__ == 2
   std::cerr << ((std::type_info*)((*__get_eh_info ())->type))->name();
#endif   
   std::cerr << '\n';
   std::cerr.flush();
   _exit(1);
}

static void print_exception_t()
{  
   std::cerr << "uncaught std::exception: ";
#if __GNUC__ == 2   
   std::cerr << ((std::type_info*)((*__get_eh_info ())->type))->name();
#endif   
   std::cerr << '\n';
   std::cerr.flush();
   _exit(1);
}

#ifdef _MSC_VER
#include "windows.h"
/* This function will be called when a trap occurs. Thanks to Jacob
   Navia for his contribution. */

static long CALLBACK 
_gnu_exception_handler (EXCEPTION_POINTERS * exception_data)
{
  long action = EXCEPTION_CONTINUE_SEARCH;
  int reset_fpu = 0;

  switch (exception_data->ExceptionRecord->ExceptionCode)
    {
    case EXCEPTION_ACCESS_VIOLATION:
      /* test if the user has set SIGSEGV */
      break;

    case EXCEPTION_ILLEGAL_INSTRUCTION:
    case EXCEPTION_PRIV_INSTRUCTION:
      /* test if the user has set SIGILL */
      break;

    case EXCEPTION_FLT_INVALID_OPERATION:
    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
    case EXCEPTION_FLT_DENORMAL_OPERAND:
    case EXCEPTION_FLT_OVERFLOW:
    case EXCEPTION_FLT_UNDERFLOW:
    case EXCEPTION_FLT_INEXACT_RESULT:
      reset_fpu = 1;
      /* fall through. */

    case EXCEPTION_INT_DIVIDE_BY_ZERO:
      /* test if the user has set SIGFPE */
      break;

    default:
      break;
    }
  return action;
}
#endif

void ManuProC::PrintUncaughtExceptions() throw()
{
#ifdef _MSC_VER
  /*
   * Set up the top-level exception handler so that signal handling
   * works as expected. The mapping between ANSI/POSIX signals and
   * Win32 SE is not 1-to-1, so caveat emptore.
   * 
   */
  SetUnhandledExceptionFilter ((LPTOP_LEVEL_EXCEPTION_FILTER)_gnu_exception_handler);
#elif __GNUC__ == 2 || ( __GNUC__ == 3 && __GNUC_MINOR__ < 2 )
   std::set_unexpected(print_exception_u);
   std::set_terminate(print_exception_t);
#elif defined(__GNUC__)
   std::set_terminate (__gnu_cxx::__verbose_terminate_handler);
   std::set_unexpected (__gnu_cxx::__verbose_terminate_handler);
#endif   
}

#if 0
What does a throw do?

it calls 
	void * __eh_alloc(size_t size)
uses a copy ctor of the std::exception and then destructs all
instances, then calls 
	void __cp_push_exception(void *value, void *type, void (*cleanup)(void *, int))
and (__eh_pc ?)
	void __throw()

#endif

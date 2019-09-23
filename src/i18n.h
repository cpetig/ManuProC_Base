/* this file contains the necessary defines to use gettext in your program */

#if defined(WIN32) && !defined(__MINGW32__)
#       pragma warning( disable : 4290 )
#	pragma once
#	include <ManuProCConfig.h>
#else
#	include <ManuProCConfig.h>
#endif

#ifdef ENABLE_NLS
#  include <libintl.h>
#  undef _
#  define _(String) dgettext (GETTEXT_PACKAGE, String)
#  ifdef gettext_noop
#    define N_(String) gettext_noop (String)
#  else
#    define N_(String) (String)
#  endif
#else
#  define textdomain(String) (String)
#  define gettext(String) (String)
#  define dgettext(Domain,Message) (Message)
#  define dcgettext(Domain,Message,Type) (Message)
#  define bindtextdomain(Domain,Directory) (Domain)
#  define _(String) (String)
#  define N_(String) (String)
#endif

#ifdef __cplusplus
namespace ManuProC_Base { void init_i18n(char const* package_locale_dir); }
#endif

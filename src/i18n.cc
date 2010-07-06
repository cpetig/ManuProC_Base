#include "i18n.h"

#ifdef ENABLE_NLS
void ManuProC_Base::init_i18n(char const* package_locale_dir)
{
   if (!package_locale_dir) package_locale_dir= PACKAGE_LOCALE_DIR;
   bindtextdomain (GETTEXT_PACKAGE, package_locale_dir);
   bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
}
#endif

#include <stdio.h>
#include <assert.h>
#include <string>
#include <map>
#include <gettext-po.h>
#include <string.h>

static void xerror (int severity,
                    po_message_t message,
                    const char *filename, size_t lineno, size_t column,
                    int multiline_p, const char *message_text)
{
  fprintf(stderr,"xerror %s:%d:%d: %s\n", filename, lineno, column, message_text);
  abort();
}

static void xerror2 (int severity,
                     po_message_t message1,
                     const char *filename1, size_t lineno1, size_t column1,
                     int multiline_p1, const char *message_text1,
                     po_message_t message2,
                     const char *filename2, size_t lineno2, size_t column2,
                     int multiline_p2, const char *message_text2)
{
  fprintf(stderr,"xerror1 %s:%d:%d: %s\n", filename1, lineno1, column1, message_text1);
  fprintf(stderr,"xerror2 %s:%d:%d: %s\n", filename2, lineno2, column2, message_text2);
  abort();
}

struct len_sorted : std::string
{
  len_sorted(std::string const& a) : std::string(a) {}
  bool operator<(len_sorted const&b) const
  { return size()>b.size() ? true : size()<b.size() ? false : strcmp(c_str(), b.c_str())<0; }
};

std::string escape(std::string const& s)
{
  std::string res;
  for (std::string::const_iterator i=s.begin();i!=s.end();++i)
     switch(*i)
     {
       case '\n': res+="\\n"; break;
       case '.': res+="\\."; break;
       case '[': res+="\\["; break;
       case ']': res+="\\]"; break;
       case '\\': res+="\\\\"; break;
//       case '(': res+="\\("; break;
//       case ')': res+="\\)"; break;
//       case '{': res+="\\{"; break;
//       case '}': res+="\\}"; break;
       case '*': res+="\\*"; break;
       case '$': res+="\\$"; break;
       case '^': res+="\\^"; break;
       case '&': res+="\\&"; break;
       default: res+=*i; break;
     }
  return res;
}

enum { ST_NONE, ST_GLADE, ST_C };

std::string sescape(std::string const& s, int style)
{
  std::string res;
  if (style==ST_GLADE)
  {
    res+=" translatable=\"yes\">";
    for (std::string::const_iterator i=s.begin();i!=s.end();++i)
     switch(*i)
     {
       case '<': res+="&lt;"; break;
       case '>': res+="&gt;"; break;
       case '&': res+="&amp;"; break;
       default: res+=*i; break;
     }
    res+="</property>";
  }
  else if (style==ST_C)
  {
    res+="_(\"";
    for (std::string::const_iterator i=s.begin();i!=s.end();++i)
     switch(*i)
     {
       case '\n': res+="\\n"; break;
       case '\t': res+="\\t"; break;
       case '\\': res+="\\\\"; break;
       default: res+=*i; break;
     }
    res+="\")";
  }
  else res=s;
  return res;
}

int main(int argc, char **argv)
{
  unsigned style=ST_NONE;
  if (argc<2)
  {
    fprintf(stderr, "USAGE: %s <po file> [style]\n", argv[0]);
    return 1;
  }
  if (argc>2) style=atoi(argv[2]);
  
  assert(setlocale(LC_CTYPE, ""));
 std::map<len_sorted,std::string> translations;

    const po_xerror_handler handler = { &xerror, &xerror2 };
    po_file_t file = po_file_read (argv[1], &handler);
    if (file == NULL)
    {
      fprintf(stderr, "couldn't open PO file %s\n", argv[1]);
      return 1;
    }
    else
    {
      const char * const *domains = po_file_domains (file);
      const char * const *domainp;
      for (domainp = domains; *domainp; domainp++)
      {
        const char *domain = *domainp;
        po_message_iterator_t iterator = po_message_iterator (file, domain);

        for (;;)
        {
          po_message_t message = po_next_message (iterator);

          if (message == NULL)
            break;
          {
            std::string msgid = po_message_msgid (message);
            std::string msgstr = po_message_msgstr (message);
            std::string msgcomment = po_message_comments (message);
            std::string extr_msgcomment = po_message_extracted_comments (message);
            std::string context;
            bool obselete = po_message_is_obsolete (message);
            bool fuzzy = po_message_is_fuzzy (message);

            if (msgid.empty()) continue;
            if (msgid==msgstr) continue;
            translations[escape(sescape(msgid,style))]=escape(sescape(msgstr,style));
          }
        }
      }
   }
//  FILE *outf=fopen("zh.sed","w");
  for (std::map<len_sorted,std::string>::const_iterator i=translations.begin();i!=translations.end();++i)
  {
      fprintf(stdout, "s@%s@%s@\n", i->first.c_str(), i->second.c_str());
  }
//    fclose(outf[j]);
  return 0;
}

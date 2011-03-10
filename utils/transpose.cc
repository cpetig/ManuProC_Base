#include <stdio.h>
#include <assert.h>
#include <string>
#include <map>
#include <gettext-po.h>
#include <string.h>
#include <set>

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

int main(int argc, char **argv)
{
  if (argc<2)
  {
    fprintf(stderr, "USAGE: %s <po file>\n", argv[0]);
    return 1;
  }
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
            translations[(msgid)]=(msgstr);
          }
        }
      }
   }
//  FILE *outf=fopen("zh.sed","w");
  po_file_t pof= po_file_create();
  assert(pof);
  po_message_iterator_t it=po_message_iterator(pof, NULL);
  assert(it);
  std::set<std::string> present;
  {
    po_message_t msg=po_message_create ();
    po_message_set_msgid(msg, "");
    po_message_set_msgstr(msg, "MIME-Version: 1.0\n"
	"Content-Type: text/plain; charset=UTF-8\n"
	"Content-Transfer-Encoding: 8bit\n");
    po_message_insert(it, msg);
    present.insert(std::string());
  }
  for (std::map<len_sorted,std::string>::const_iterator i=translations.begin();i!=translations.end();++i)
  {
//      fprintf(stdout, "s@%s@%s@\n", i->first.c_str(), i->second.c_str());
    std::string msgid= i->second;
    std::string transl= i->first;
    if (present.find(msgid)!=present.end()) continue;
    present.insert(msgid);
    po_message_t msg=po_message_create ();
    po_message_set_msgid(msg, msgid.c_str());
    po_message_set_msgstr(msg, transl.c_str());
    po_message_insert(it, msg);
  }
  po_file_write(pof, "/dev/stdout", &handler);
  po_file_free(pof);
  return 0;
}

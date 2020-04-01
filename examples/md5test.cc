#include "md5string.h"

int main(int argc, char **argv)
{
    std::string m = md5("testpass");
    for (uint32_t i=0;i<m.size();++i)
       printf("%02x", m[i]&0xff);
    printf("\n");
    return 0;
}

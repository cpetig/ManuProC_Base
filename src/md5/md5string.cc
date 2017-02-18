#include <string>

extern "C" {
#include "global.h"
#include "md5.h"
}
#include "../md5string.h"

std::string md5(std::string const& in)
{
  MD5_CTX context;
  unsigned char digest[16];
  MD5Init(&context);
  MD5Update(&context, (unsigned char*)in.data(), in.size());
  MD5Final(digest, &context);
  return std::string(digest, digest+sizeof(digest));
}

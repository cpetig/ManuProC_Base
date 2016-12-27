
#include <Misc/Query.h>
#include <iostream>
#include <sstream>

int main()
{
  SQLerror e("test",100,"test2");
  std::cout << e << '\n';
  
  std::stringstream ss;
  ss << e;
  
  return 0;
}

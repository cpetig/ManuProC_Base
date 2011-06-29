#include <iostream>
#include <Misc/Datum.h>
#include <Misc/create_parse.h>

int main()
{
  std::cout << ManuProC::Datum::from_access("14.04.2012 00:00:00") << '\n';
  std::cout << ManuProC::parse<float>("-24.8") << '\n';
  return 0;
}

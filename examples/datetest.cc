#include <iostream>
#include <Misc/Datum.h>

int main()
{
  std::cout << ManuProC::Datum::from_access("14.04.2012 00:00:00") << '\n';
  return 0;
}
